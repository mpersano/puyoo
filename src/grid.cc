#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "sprite.h"
#include "font.h"
#include "falling_block.h"
#include "grid.h"

enum {
	MIN_CHAIN_SIZE = 3,

	DROPPING_BLOCK_TICS = 10,
	EXPLODING_BLOCK_TICS = 12,

	DROPPING_JAMA_SPEED = 5,

	NUM_EXPLOSION_FRAMES = 4,
};

static const sprite_atlas *sprites;

const sprite *block_sprites[NUM_BLOCK_TYPES];

static const sprite *explosion_sprites[NUM_EXPLOSION_FRAMES];
static const sprite *jama_sprite;
static const sprite *grid_background_sprite;
static const font *hud_font;

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
initialize_jama_sprites()
{
	jama_sprite = sprites->get_sprite("jama.png");
}

static void
initialize_background_sprites()
{
	grid_background_sprite = sprites->get_sprite("grid-background.png");
}

void
grid_init_resources()
{
	sprites = sprite_atlas_manager::instance().get("SPRITES");
	initialize_block_sprites();
	initialize_explosion_sprites();
	initialize_jama_sprites();
	initialize_background_sprites();

	hud_font = font_manager::instance().get("SONIC");
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

grid::grid(int base_x, int base_y, grid *opponent, bool human_control)
: falling_block_(human_control)
, base_x_(base_x)
, base_y_(base_y)
, opponent_(opponent)
{
	reset();
}

void
grid::reset()
{
	memset(blocks_, 0, sizeof(blocks_));

	jama_to_drop_ = combo_size_ = 0;

	set_state(STATE_PLAYER_CONTROL);

	falling_block_.reset();
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
			const int type = *p;

			switch (type) {
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
					block_sprites[type - 1]->draw(gfx, x, hanging ? y + y_offset : y);
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
	const rect clip_rect(base_x_, base_x_ + GRID_COLS*BLOCK_SIZE, base_y_, base_y_ + GRID_ROWS*BLOCK_SIZE);

	int x = base_x_;

	for (int i = 0; i < GRID_COLS; i++) {
		int num_jama = dropping_jama_[i];

		if (num_jama) {
			int height = base_y_ + (GRID_ROWS - (get_col_height(i) + dropping_jama_[i]))*BLOCK_SIZE;

			int y = base_y_ - num_jama*BLOCK_SIZE + DROPPING_JAMA_SPEED*state_tics_;
			if (y > height)
				y = height;

			if (y <= base_y_ - BLOCK_SIZE) {
				int n = (base_y_ - y)/BLOCK_SIZE;
				y += n*BLOCK_SIZE;
				num_jama -= n;
			}

			for (int j = 0; j < num_jama; j++) {
				jama_sprite->draw(gfx, clip_rect, x, y);
				y += BLOCK_SIZE;
			}
		}

		x += BLOCK_SIZE;
	}
}

void
grid::draw_hud(gfx::context& gfx) const
{
	hud_font->draw(gfx, base_x_, base_y_ - 8, "jama: %d", jama_to_drop_);
}

void
grid::draw_background(gfx::context& gfx) const
{
	grid_background_sprite->draw(gfx, base_x_, base_y_);
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
				if (++combo_size_ >= 2)
					combo_animation_.reset(combo_size_);
				chain_explode(r, c, type);
				found = true;
			}
		}
	}

	return found;
}

bool
grid::is_game_over() const
{
	return blocks_[(GRID_ROWS - 1)*GRID_COLS + 2] != BLOCK_EMPTY || blocks_[(GRID_ROWS - 1)*GRID_COLS + 3] != BLOCK_EMPTY;
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
	gfx.bind_texture(sprites->get_texture());

	draw_background(gfx);

	draw_blocks(gfx);

	combo_animation_.draw(gfx, base_x_, base_y_);

	switch (state_) {
		case STATE_PLAYER_CONTROL:
			falling_block_.draw(gfx, base_x_, base_y_);
			break;

		case STATE_DROPPING_JAMA:
			draw_dropping_jama(gfx);
			break;

		default:
			break;
	}

	draw_hud(gfx);
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
		} else if (is_game_over()) {
			set_state(STATE_GAME_OVER);
		} else {
			opponent_->add_jama(combo_size_);
			falling_block_.reset();
			combo_size_ = 0;
			set_state(STATE_PLAYER_CONTROL);
		}
	};

	combo_animation_.update();

	switch (state_) {
		case STATE_PLAYER_CONTROL:
			if (!falling_block_.update(this, dpad_state)) {
				falling_block_.copy_to_grid(this);
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

		case STATE_GAME_OVER:
			break;

		default:
			break;
	}
}

void
grid::add_jama(int num_jama)
{
	jama_to_drop_ += num_jama;
}

bool
grid::game_over() const
{
	return state_ == STATE_GAME_OVER;
}

void
grid::drop_jama()
{
	memset(dropping_jama_, 0, sizeof(dropping_jama_));

	while (jama_to_drop_) {
		int col = -1, index = 1;

		for (int j = 0; j < GRID_COLS; j++) {
			int height = get_col_height(j) + dropping_jama_[j];

			if (height < GRID_ROWS) {
				if (rand()%index == 0)
					col = j;
				++index;
			}
		}

		if (col == -1)
			break;

		dropping_jama_[col]++;
		--jama_to_drop_;
	}

	jama_drop_tics_ = 0;

	for (int i = 0; i < GRID_COLS; i++) {
		int height = (GRID_ROWS - get_col_height(i))*BLOCK_SIZE;
		int tics = height/DROPPING_JAMA_SPEED;

		if (tics > jama_drop_tics_)
			jama_drop_tics_ = tics;
	}
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
