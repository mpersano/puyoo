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
	EXPLODING_BLOCK_TICS = 5,

	FALLING_BLOCK_UPDATE_INTERVAL = 3,
	FALLING_BLOCK_DROP_INTERVAL = 30,
	FALLING_BLOCK_NUM_ROTATIONS = 4,
	FALLING_BLOCK_ROTATION_TICS = 8,
};

static const sprite_atlas *sprites;
static const sprite *block_sprites[NUM_BLOCK_TYPES - 2]; // HACK

void
grid_load_sprites()
{
	sprites = sprite_atlas_manager::instance().get("SPRITES");
	sprites->get_texture()->upload_to_vram();

	static const char *names[NUM_BLOCK_TYPES - 2] = { "red.png", "green.png", "blue.png" };
	for (int i = 0; i < NUM_BLOCK_TYPES - 2; i++) {
		block_sprites[i] = sprites->get_sprite(names[i]);
		printf("%s -> %p\n", names[i], block_sprites[i]);
	}
}

static void
block_draw(gfx::context& gfx, int type, int x, int y)
{
	if (type >= BLOCK_RED && type <= BLOCK_BLUE) { // HACK
#if 1
		gfx.add_rectangle(x, y, 16, 16, gfx::rgb(255, 255, 255));
#else
		block_sprites[type - 1]->draw(gfx, x, y);
#endif
	}
}

static const int offsets[FALLING_BLOCK_NUM_ROTATIONS][2] = { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } };

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
grid::falling_block::initialize()
{
	blocks_[0] = rand()%(NUM_BLOCK_TYPES - 2) + 1;
	blocks_[1] = rand()%(NUM_BLOCK_TYPES - 2) + 1;

	row_ = GRID_ROWS - 1;
	col_ = GRID_COLS/2 - 1;
	rotation_ = 0;

	input_poll_tics_ = 0;
	drop_tics_ = FALLING_BLOCK_DROP_INTERVAL;

	set_state(STATE_PLAYER_CONTROL);
}

void
grid::falling_block::draw(gfx::context& gfx, int base_x, int base_y) const
{
	// first block
	block_draw(gfx, blocks_[0], base_x + col_*BLOCK_SIZE, base_y + (GRID_ROWS - 1)*BLOCK_SIZE - row_*BLOCK_SIZE);

	// second block
	int x, y;

	if (state_ != STATE_ROTATING) {
		x = base_x + (col_ + offsets[rotation_][1])*BLOCK_SIZE;
		y = base_y + (GRID_ROWS - 1)*BLOCK_SIZE - (row_ + offsets[rotation_][0])*BLOCK_SIZE;
	} else {
		static const struct offset {
			int dx, dy;
		} rotation_offsets[4][FALLING_BLOCK_ROTATION_TICS] = {
			{ { 16, 0 }, { 15, 3 }, { 14, 6 }, { 13, 8 }, { 11, 11 }, { 8, 13 }, { 6, 14 }, { 3, 15 }, },
			{ { 0, 16 }, { -3, 15 }, { -6, 14 }, { -8, 13 }, { -11, 11 }, { -13, 8 }, { -14, 6 }, { -15, 3 }, },
			{ { -16, 0 }, { -15, -3 }, { -14, -6 }, { -13, -8 }, { -11, -11 }, { -8, -13 }, { -6, -14 }, { -3, -15 }, },
			{ { 0, -16 }, { 3, -15 }, { 6, -14 }, { 8, -13 }, { 11, -11 }, { 13, -8 }, { 14, -6 }, { 15, -3 }, },
		};

		int xo = base_x + col_*BLOCK_SIZE;
		int yo = base_y + (GRID_ROWS - 1)*BLOCK_SIZE - row_*BLOCK_SIZE;

		x = xo + rotation_offsets[rotation_][state_tics_].dx;
		y = yo + rotation_offsets[rotation_][state_tics_].dy;
	}

	block_draw(gfx, blocks_[1], x, y);
}

bool
grid::falling_block::can_move(const grid *g, int dr, int dc) const
{
	return
		g->is_empty(row_ + dr, col_ + dc) &&
		g->is_empty(row_ + offsets[rotation_][0] + dr, col_ + offsets[rotation_][1] + dc);
}

bool
grid::falling_block::update(const grid *g, unsigned dpad_state)
{
	if (state_ == STATE_PLAYER_CONTROL) {
		bool is_active = true;

		if (input_poll_tics_ > 0) {
			--input_poll_tics_;
		} else {
			if (dpad_state & DPAD_LEFT) {
				if (can_move(g, 0, -1)) {
					--col_;
					input_poll_tics_ = FALLING_BLOCK_UPDATE_INTERVAL;
				}
			}

			if (dpad_state & DPAD_RIGHT) {
				if (can_move(g, 0, 1)) {
					++col_;
					input_poll_tics_ = FALLING_BLOCK_UPDATE_INTERVAL;
				}
			}

			if (dpad_state & DPAD_DOWN) {
				if (can_move(g, -1, 0)) {
					--row_;
					input_poll_tics_ = FALLING_BLOCK_UPDATE_INTERVAL;
				} else {
					is_active = false;
				}
			}

			if (dpad_state & DPAD_BUTTON) {
				int next_rotation = rotation_ + 1;
				if (next_rotation == FALLING_BLOCK_NUM_ROTATIONS)
					next_rotation = 0;

				if (g->is_empty(row_ + offsets[next_rotation][0], col_ + offsets[next_rotation][1]))
					set_state(STATE_ROTATING);
			}
		}

		if (is_active && state_ == STATE_PLAYER_CONTROL) {
			if (drop_tics_ > 0) {
				--drop_tics_;
			} else {
				if (can_move(g, -1, 0)) {
					--row_;
					drop_tics_ = FALLING_BLOCK_DROP_INTERVAL;
				} else {
					/* can't drop */
					is_active = false;
				}
			}
		}

		return is_active;
	} else if (state_ == STATE_ROTATING) {
		if (++state_tics_ == FALLING_BLOCK_ROTATION_TICS) {
			if (++rotation_ == FALLING_BLOCK_NUM_ROTATIONS)
				rotation_ = 0;

			set_state(STATE_PLAYER_CONTROL);
		}

		return true;
	}

	// NOTREACHED (?)
	return false;
}

void
grid::falling_block::set_state(state next_state)
{
	state_ = next_state;
	state_tics_ = 0;
}

void
grid::falling_block::copy_to_grid(grid *g)
{
	g->set_block(row_, col_, blocks_[0]);
	g->set_block(row_ + offsets[rotation_][0], col_ + offsets[rotation_][1], blocks_[1]);
}

void
grid::set_state(state next_state)
{
	state_ = next_state;
	state_tics_ = 0;
}

void
grid::initialize(int base_x, int base_y)
{
	base_x_ = base_x;
	base_y_ = base_y;

	memset(blocks_, 0, sizeof(blocks_));

	set_state(STATE_PLAYER_CONTROL);
	falling_block_.initialize();
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
			if (*p == BLOCK_EMPTY)
				hanging = true;
			else
				block_draw(gfx, *p, x, hanging ? y + y_offset : y);

			y -= BLOCK_SIZE;
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
	if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS && get_block(r, c) == type) {
		set_block(r, c, BLOCK_EXPLODING);

		chain_explode(r - 1, c, type);
		chain_explode(r + 1, c, type);
		chain_explode(r, c - 1, type);
		chain_explode(r, c + 1, type);
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

		if (type != BLOCK_EMPTY) {
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
			falling_block_.draw(gfx, base_x_, base_y_);
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
		} else {
			set_state(STATE_PLAYER_CONTROL);
			falling_block_.initialize();
		}
	};

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

		default:
			break;
	}
}
