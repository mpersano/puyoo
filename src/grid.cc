#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "texture.h"
#include "sprite.h"
#include "grid.h"

enum {
	MIN_CHAIN_SIZE = 3,

	DROPPING_BLOCK_TICS = 10,
	EXPLODING_BLOCK_TICS = 12,

	FALLING_BLOCK_UPDATE_INTERVAL = 3,
	FALLING_BLOCK_DROP_INTERVAL = 30,
	FALLING_BLOCK_NUM_ROTATIONS = 4,

	FALLING_BLOCK_ANIMATION_TICS = 4,
	FALLING_BLOCK_WAIT_TICS = 2,
	FALLING_BLOCK_ROTATION_TICS = 8,

	DROPPING_JAMA_SPEED = 5,

	NUM_EXPLOSION_FRAMES = 4,
};

static const sprite_atlas *sprites;

static const sprite *block_sprites[NUM_BLOCK_TYPES];
static const sprite *explosion_sprites[NUM_EXPLOSION_FRAMES];
static const sprite *jama_sprite;

static const int offsets[FALLING_BLOCK_NUM_ROTATIONS][2] = { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } };

static void
initialize_block_sprites()
{
	static const char *names[NUM_BLOCK_TYPES] = {
		"cyan.png",
		"red.png",
		"blue.png",
		"green.png",
		"purple.png",
		"yellow.png",
	};

	for (int i = 0; i < NUM_BLOCK_TYPES; i++) {
		block_sprites[i] = sprites->get_sprite(names[i]);
		printf("%s -> %p\n", names[i], block_sprites[i]);
	}
}

static void
initialize_explosion_sprites()
{
	for (int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
		char name[80];
		sprintf(name, "explosion-%d.png", i);
		explosion_sprites[i] = sprites->get_sprite(name);
	}
}

static void
initialize_jama_sprite()
{
	jama_sprite = sprites->get_sprite("jama.png");
}

void
grid_load_sprites()
{
	sprites = sprite_atlas_manager::instance().get("SPRITES");
	sprites->get_texture()->upload_to_vram();

	initialize_block_sprites();
	initialize_explosion_sprites();
	initialize_jama_sprite();
}

static void
block_draw(gfx::context& gfx, int type, int x, int y)
{
	if (type >= BLOCK_EMPTY + 1 && type <= BLOCK_EXPLODING - 1)
		block_sprites[type - 1]->draw(gfx, x, y);
}

class control_strategy
{
public:
	virtual ~control_strategy() { }

	virtual bool update(falling_block *fb, const grid *g, unsigned dpad_state) = 0;
};

class falling_block
{
public:
	falling_block(bool player_control);
	~falling_block();

	void reset();
	void draw(gfx::context& gfx, int base_x, int base_y) const;
	bool update(const grid *g, unsigned dpad_state);
	bool can_move(const grid *g, int dr, int dc) const;
	void copy_to_grid(grid *g);

	bool move_left(const grid *g);
	bool move_right(const grid *g);
	bool move_down(const grid *g);
	bool rotate(const grid *g);

protected:
	enum state {
		STATE_PLAYER_CONTROL,
		STATE_ROTATING,
		STATE_MOVING_LEFT,
		STATE_MOVING_RIGHT,
		STATE_DROPPING,
		STATE_WAITING,
	};

	void set_state(state next_state);

	int blocks_[2];
	int row_, col_, rotation_;
	int drop_tics_;
	state state_;
	int state_tics_;
	control_strategy *control_strategy_;
};

class player_control_strategy : public control_strategy
{
public:
	bool update(falling_block *fb, const grid *g, unsigned dpad_state);
};

class computer_control_strategy : public control_strategy
{
public:
	bool update(falling_block *fb, const grid *g, unsigned dpad_state);
};

falling_block::falling_block(bool player_control)
: control_strategy_(
	player_control ? static_cast<control_strategy *>(new player_control_strategy) :
			 static_cast<control_strategy *>(new computer_control_strategy))
{
	reset();
}

falling_block::~falling_block()
{
	delete control_strategy_;
}

void
falling_block::reset()
{
	blocks_[0] = rand()%NUM_BLOCK_TYPES + 1;
	blocks_[1] = rand()%NUM_BLOCK_TYPES + 1;

	row_ = GRID_ROWS - 1;
	col_ = GRID_COLS/2 - 1;
	rotation_ = 0;

	drop_tics_ = FALLING_BLOCK_DROP_INTERVAL;

	set_state(STATE_PLAYER_CONTROL);
}

void
falling_block::draw(gfx::context& gfx, int base_x, int base_y) const
{
	const int xo = base_x + col_*BLOCK_SIZE;
	const int yo = base_y + (GRID_ROWS - 1)*BLOCK_SIZE - row_*BLOCK_SIZE;

	// first block
	int x0, y0;

	switch (state_) {
		case STATE_MOVING_LEFT:
			x0 = xo - state_tics_*BLOCK_SIZE/FALLING_BLOCK_ANIMATION_TICS;
			y0 = yo;
			break;

		case STATE_MOVING_RIGHT:
			x0 = xo + state_tics_*BLOCK_SIZE/FALLING_BLOCK_ANIMATION_TICS;
			y0 = yo;
			break;

		case STATE_DROPPING:
			x0 = xo;
			y0 = yo + state_tics_*BLOCK_SIZE/FALLING_BLOCK_ANIMATION_TICS;
			break;

		default:
			x0 = xo;
			y0 = yo;
			break;
	}

	// second block
	int x1, y1;

	if (state_ != STATE_ROTATING) {
		x1 = x0 + offsets[rotation_][1]*BLOCK_SIZE;
		y1 = y0 - offsets[rotation_][0]*BLOCK_SIZE;
	} else {
		static const struct offset {
			int dx, dy;
		} rotation_offsets[4][FALLING_BLOCK_ROTATION_TICS] = {
			{ { 16, 0 }, { 15, 3 }, { 14, 6 }, { 13, 8 }, { 11, 11 }, { 8, 13 }, { 6, 14 }, { 3, 15 }, },
			{ { 0, 16 }, { -3, 15 }, { -6, 14 }, { -8, 13 }, { -11, 11 }, { -13, 8 }, { -14, 6 }, { -15, 3 }, },
			{ { -16, 0 }, { -15, -3 }, { -14, -6 }, { -13, -8 }, { -11, -11 }, { -8, -13 }, { -6, -14 }, { -3, -15 }, },
			{ { 0, -16 }, { 3, -15 }, { 6, -14 }, { 8, -13 }, { 11, -11 }, { 13, -8 }, { 14, -6 }, { 15, -3 }, },
		};

		x1 = x0 + rotation_offsets[rotation_][state_tics_].dx;
		y1 = y0 + rotation_offsets[rotation_][state_tics_].dy;
	}

	block_draw(gfx, blocks_[0], x0, y0);
	block_draw(gfx, blocks_[1], x1, y1);
}

bool
falling_block::can_move(const grid *g, int dr, int dc) const
{
	return
		g->is_empty(row_ + dr, col_ + dc) &&
		g->is_empty(row_ + offsets[rotation_][0] + dr, col_ + offsets[rotation_][1] + dc);
}

bool
falling_block::update(const grid *g, unsigned dpad_state)
{
	switch (state_) {
		case STATE_PLAYER_CONTROL:
			if (!control_strategy_->update(this, g, dpad_state))
				return false;

			if (state_ == STATE_PLAYER_CONTROL) {
				if (drop_tics_ > 0) {
					--drop_tics_;
				} else {
					if (can_move(g, -1, 0)) {
						set_state(STATE_DROPPING);
						return true;
					} else {
						/* can't drop */
						return false;
					}
				}
			}

			return true;

		case STATE_MOVING_LEFT:
			if (++state_tics_ == FALLING_BLOCK_ANIMATION_TICS) {
				--col_;
				set_state(STATE_WAITING);
			}
			return true;

		case STATE_MOVING_RIGHT:
			if (++state_tics_ == FALLING_BLOCK_ANIMATION_TICS) {
				++col_;
				set_state(STATE_WAITING);
			}
			return true;

		case STATE_WAITING:
			if (++state_tics_ == FALLING_BLOCK_WAIT_TICS)
				set_state(STATE_PLAYER_CONTROL);
			return true;

		case STATE_DROPPING:
			if (++state_tics_ == FALLING_BLOCK_ANIMATION_TICS) {
				--row_;
				drop_tics_ = FALLING_BLOCK_DROP_INTERVAL;
				set_state(STATE_PLAYER_CONTROL);
			}
			return true;

		case STATE_ROTATING:
			if (++state_tics_ == FALLING_BLOCK_ROTATION_TICS) {
				if (++rotation_ == FALLING_BLOCK_NUM_ROTATIONS)
					rotation_ = 0;

				set_state(STATE_PLAYER_CONTROL);
			}
			return true;

		default:
			// NOTREACHED (?)
			return false;
	}
}

bool
falling_block::move_left(const grid *g)
{
	if (can_move(g, 0, -1)) {
		set_state(STATE_MOVING_LEFT);
		return true;
	}

	return false;
}

bool
falling_block::move_right(const grid *g)
{
	if (can_move(g, 0, 1)) {
		set_state(STATE_MOVING_RIGHT);
		return true;
	}

	return false;
}

bool
falling_block::move_down(const grid *g)
{
	if (can_move(g, -1, 0)) {
		set_state(STATE_DROPPING);
		return true;
	}

	return false;
}

bool
falling_block::rotate(const grid *g)
{
	int next_rotation = rotation_ + 1;
	if (next_rotation == FALLING_BLOCK_NUM_ROTATIONS)
		next_rotation = 0;

	if (g->is_empty(row_ + offsets[next_rotation][0], col_ + offsets[next_rotation][1])) {
		set_state(STATE_ROTATING);
		return true;
	}

	return false;
}

bool
player_control_strategy::update(falling_block *fb, const grid *g, unsigned dpad_state)
{
	if (dpad_state & DPAD_LEFT) {
		if (fb->move_left(g))
			return true;
	}

	if (dpad_state & DPAD_RIGHT) {
		if (fb->move_right(g))
			return true;
	}

	if (dpad_state & DPAD_BUTTON) {
		if (fb->rotate(g))
			return true;
	}

	if (dpad_state & DPAD_DOWN)
		return fb->move_down(g);

	return true;
}

bool
computer_control_strategy::update(falling_block *fb, const grid *g, unsigned dpad_state)
{
	if ((rand() & 0x3f) == 0) {
		if (fb->move_left(g))
			return true;
	}

	if ((rand() & 0x3f) == 0) {
		if (fb->move_right(g))
			return true;
	}

	if ((rand() & 0x3f) == 0) {
		if (fb->rotate(g))
			return true;
	}

	if ((rand() & 0x3f) == 0)
		return fb->move_down(g);

	return true;
}

void
falling_block::set_state(state next_state)
{
	state_ = next_state;
	state_tics_ = 0;
}

void
falling_block::copy_to_grid(grid *g)
{
	g->set_block(row_, col_, blocks_[0]);
	g->set_block(row_ + offsets[rotation_][0], col_ + offsets[rotation_][1], blocks_[1]);
}

int
grid::get_block(int r, int c) const
{
	return blocks_[r*GRID_COLS + c];
}

bool
grid::is_empty(int r, int c) const
{
	return r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS && get_block(r, c) == BLOCK_EMPTY;
}

void
grid::set_block(int r, int c, int type)
{
	blocks_[r*GRID_COLS + c] = type;
}

void
grid::set_state(state next_state)
{
	state_ = next_state;
	state_tics_ = 0;
}

void
grid::initialize(int base_x, int base_y, bool human_control)
{
	base_x_ = base_x;
	base_y_ = base_y;

	falling_block_ = new falling_block(human_control);

	reset();
}

void
grid::reset()
{
	memset(blocks_, 0, sizeof(blocks_));

	jama_to_drop_ = 9; // 0;

	set_state(STATE_PLAYER_CONTROL);
	falling_block_->reset();
}

void
grid::draw_blocks(gfx::context& gfx) const
{
	int y_offset;

	if (state_ == STATE_DROPPING_BLOCKS)
		y_offset = state_tics_*BLOCK_SIZE/DROPPING_BLOCK_TICS;
	else
		y_offset = 0;

	int x = base_x_;

	for (int c = 0; c < GRID_COLS; c++) {
		bool hanging = false;

		int y = base_y_ + (GRID_ROWS - 1)*BLOCK_SIZE;

		for (const unsigned char *p = &blocks_[c]; p < &blocks_[GRID_ROWS*GRID_COLS]; p += GRID_COLS) {
			switch (*p) {
				case BLOCK_EMPTY:
					hanging = true;
					break;

				case BLOCK_EXPLODING:
					{
					const int frame = (state_tics_*NUM_EXPLOSION_FRAMES)/EXPLODING_BLOCK_TICS;
					explosion_sprites[frame]->draw(gfx, x, y);
					}
					break;

				case BLOCK_JAMA:
					jama_sprite->draw(gfx, x, hanging ? y + y_offset : y);
					break;

				default:
					block_draw(gfx, *p, x, hanging ? y + y_offset : y);
					break;
			}

			y -= BLOCK_SIZE;
		}

		x += BLOCK_SIZE;
	}
}

int
grid::get_col_height(int c) const
{
	int h = 0;

	while (h < GRID_ROWS && blocks_[c + h*GRID_COLS] != BLOCK_EMPTY)
		++h;

	return h;
}

void
grid::draw_dropping_jama(gfx::context& gfx) const
{
	int x = base_x_;

	for (int i = 0; i < GRID_COLS; i++) {
		int num_jama = dropping_jama_[i];

		if (num_jama) {
			int height = base_y_ + GRID_ROWS*BLOCK_SIZE - (get_col_height(i) + dropping_jama_[i])*BLOCK_SIZE;

			int y = base_y_ - num_jama*BLOCK_SIZE + DROPPING_JAMA_SPEED*state_tics_;
			if (y > height)
				y = height;

			if (y <= base_y_ - BLOCK_SIZE) {
				int n = (base_y_ - y)/BLOCK_SIZE;
				y += n*BLOCK_SIZE;
				num_jama -= n;
			}

			for (int j = 0; j < num_jama; j++) {
				jama_sprite->draw(gfx, x, y);
				y += BLOCK_SIZE;
			}
		}

		x += BLOCK_SIZE;
	}
}

void
grid::draw_background(gfx::context& gfx) const
{
	gfx.add_rectangle(base_x_, base_y_, GRID_COLS*BLOCK_SIZE, GRID_ROWS*BLOCK_SIZE, gfx::rgb(0, 0, 80));
}

void
grid::chain_explode(int r, int c, int type)
{
	if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
		int b = get_block(r, c);

		if (b == type || b == BLOCK_JAMA)
			set_block(r, c, BLOCK_EXPLODING);

		if (b == type) {
			chain_explode(r - 1, c, type);
			chain_explode(r + 1, c, type);
			chain_explode(r, c - 1, type);
			chain_explode(r, c + 1, type);
		}
	}
}

void
grid::clear_exploding_blocks()
{
	for (unsigned char *p = blocks_; p != &blocks_[GRID_ROWS*GRID_COLS]; p++)
		if (*p == BLOCK_EXPLODING)
			*p = BLOCK_EMPTY;
}

int
grid::find_chain_size(bool *visited, int r, int c, int type) const
{
	int rv = 0;

	if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS && !visited[r*GRID_COLS + c] && get_block(r, c) == type) {
		visited[r*GRID_COLS + c] = true;

		rv = 1 +
		  find_chain_size(visited, r - 1, c, type) +
		  find_chain_size(visited, r + 1, c, type) +
		  find_chain_size(visited, r, c - 1, type) +
		  find_chain_size(visited, r, c + 1, type);
	}

	return rv;
}

bool
grid::find_chains()
{
	static bool visited[GRID_ROWS*GRID_COLS];
	memset(visited, 0, sizeof(visited));

	bool found = false;

	for (int i = 0; i < GRID_ROWS*GRID_COLS; i++) {
		if (visited[i])
			continue;

		int type = blocks_[i];

		if (type != BLOCK_EMPTY && type != BLOCK_JAMA && type != BLOCK_EXPLODING) {
			const int r = i/GRID_COLS;
			const int c = i%GRID_COLS;

			int chain_size = find_chain_size(visited, r, c, type);

			if (chain_size >= MIN_CHAIN_SIZE) {
				chain_explode(r, c, type);
				found = true;
			}
		}
	}

	return found;
}

bool
grid::has_hanging_blocks() const
{
	for (const unsigned char *p = &blocks_[GRID_COLS]; p < &blocks_[GRID_ROWS*GRID_COLS]; p++) {
		if (*p != BLOCK_EMPTY && p[-GRID_COLS] == BLOCK_EMPTY)
			return true;
	}

	return false;
}

void
grid::drop_hanging_blocks()
{
	for (int c = 0; c < GRID_COLS; c++) {
		bool hanging = false;

		for (unsigned char *p = &blocks_[c]; p < &blocks_[GRID_ROWS*GRID_COLS]; p += GRID_COLS) {
			if (*p == BLOCK_EMPTY) {
				hanging = true;
			} else if (hanging) {
				p[-GRID_COLS] = *p;
				*p = BLOCK_EMPTY;
			}
		}
	}
}

void
grid::draw(gfx::context& gfx) const
{
	draw_background(gfx);

	gfx.bind_texture(sprites->get_texture());

	draw_blocks(gfx);

	switch (state_) {
		case STATE_PLAYER_CONTROL:
			falling_block_->draw(gfx, base_x_, base_y_);
			break;

		case STATE_DROPPING_JAMA:
			draw_dropping_jama(gfx);
			break;

		default:
			break;
	}
}

void
grid::update(unsigned dpad_state)
{
	auto on_drop = [this]() {
		if (has_hanging_blocks()) {
			set_state(STATE_DROPPING_BLOCKS);
		} else if (find_chains()) {
			set_state(STATE_EXPLODING_BLOCKS);
		} else if (jama_to_drop_) {
			drop_jama();
			set_state(STATE_DROPPING_JAMA);
		} else {
			falling_block_->reset();
			set_state(STATE_PLAYER_CONTROL);
		}
	};

	switch (state_) {
		case STATE_PLAYER_CONTROL:
			if (!falling_block_->update(this, dpad_state)) {
				falling_block_->copy_to_grid(this);
				on_drop();
			}
			break;

		case STATE_EXPLODING_BLOCKS:
			if (++state_tics_ == EXPLODING_BLOCK_TICS) {
				clear_exploding_blocks();
				on_drop();
			}
			break;

		case STATE_DROPPING_BLOCKS:
			if (++state_tics_ == DROPPING_BLOCK_TICS) {
				drop_hanging_blocks();
				on_drop();
			}
			break;

		case STATE_DROPPING_JAMA:
			if (++state_tics_ == jama_drop_tics_) {
				copy_jama_to_grid();
				on_drop();
			}
			break;

		default:
			break;
	}
}

void
grid::add_jama(int num_jama)
{
	jama_to_drop_++;
}

void
grid::drop_jama()
{
	memset(dropping_jama_, 0, sizeof(dropping_jama_));

	jama_drop_tics_ = 0;

	for (int i = 0; i < jama_to_drop_; i++) {
		// XXX: check height
		dropping_jama_[rand()%GRID_COLS]++;
	}

	for (int i = 0; i < GRID_COLS; i++) {
		int height = base_y_ + GRID_ROWS*BLOCK_SIZE - (get_col_height(i) + dropping_jama_[i])*BLOCK_SIZE;
		int tics = height/DROPPING_JAMA_SPEED;

		if (tics > jama_drop_tics_)
			jama_drop_tics_ = tics;
	}

	jama_to_drop_ = 0;
}

void
grid::copy_jama_to_grid()
{
	for (int i = 0; i < GRID_COLS; i++) {
		int r = get_col_height(i);

		for (int j = 0; j < dropping_jama_[i]; j++) {
			if (j + r >= GRID_ROWS) // XXX: shouldn't need this
				break;

			blocks_[(j + r)*GRID_COLS + i] = BLOCK_JAMA;
		}
	}
}
