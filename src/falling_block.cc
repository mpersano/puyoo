#include <stdlib.h>

#include "common.h"
#include "sprite.h"
#include "grid.h"
#include "falling_block.h"

enum {
	UPDATE_INTERVAL = 3,
	DROP_INTERVAL = 30,
	NUM_ROTATIONS = 4,

	ANIMATION_TICS = 6,
	WAIT_TICS = 2,
	ROTATION_TICS = 8,
};

static const int offsets[NUM_ROTATIONS][2] = { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } };

extern const sprite *block_sprites[NUM_BLOCK_TYPES];

class control_strategy
{
public:
	virtual ~control_strategy() { }

	virtual bool update(falling_block *fb, const grid *g, unsigned dpad_state) = 0;
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

	row_ = GRID_ROWS;
	col_ = GRID_COLS/2 - 1;
	rotation_ = 0;

	drop_tics_ = DROP_INTERVAL;

	set_state(STATE_DROPPING);
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
			x0 = xo - state_tics_*BLOCK_SIZE/ANIMATION_TICS;
			y0 = yo;
			break;

		case STATE_MOVING_RIGHT:
			x0 = xo + state_tics_*BLOCK_SIZE/ANIMATION_TICS;
			y0 = yo;
			break;

		case STATE_DROPPING:
			x0 = xo;
			y0 = yo + state_tics_*BLOCK_SIZE/ANIMATION_TICS;
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
		} rotation_offsets[4][ROTATION_TICS] = {
			{ { 16, 0 }, { 15, 3 }, { 14, 6 }, { 13, 8 }, { 11, 11 }, { 8, 13 }, { 6, 14 }, { 3, 15 }, },
			{ { 0, 16 }, { -3, 15 }, { -6, 14 }, { -8, 13 }, { -11, 11 }, { -13, 8 }, { -14, 6 }, { -15, 3 }, },
			{ { -16, 0 }, { -15, -3 }, { -14, -6 }, { -13, -8 }, { -11, -11 }, { -8, -13 }, { -6, -14 }, { -3, -15 }, },
			{ { 0, -16 }, { 3, -15 }, { 6, -14 }, { 8, -13 }, { 11, -11 }, { 13, -8 }, { 14, -6 }, { 15, -3 }, },
		};

		x1 = x0 + rotation_offsets[rotation_][state_tics_].dx;
		y1 = y0 + rotation_offsets[rotation_][state_tics_].dy;
	}

	const rect clip_rect(base_x, base_x + GRID_COLS*BLOCK_SIZE, base_y, base_y + GRID_ROWS*BLOCK_SIZE);
	block_sprites[blocks_[0] - 1]->draw(gfx, clip_rect, x0, y0);
	block_sprites[blocks_[1] - 1]->draw(gfx, clip_rect, x1, y1);
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
			if (++state_tics_ == ANIMATION_TICS) {
				--col_;
				set_state(STATE_WAITING);
			}
			return true;

		case STATE_MOVING_RIGHT:
			if (++state_tics_ == ANIMATION_TICS) {
				++col_;
				set_state(STATE_WAITING);
			}
			return true;

		case STATE_WAITING:
			if (++state_tics_ == WAIT_TICS)
				set_state(STATE_PLAYER_CONTROL);
			return true;

		case STATE_DROPPING:
			if (++state_tics_ == ANIMATION_TICS) {
				--row_;
				drop_tics_ = DROP_INTERVAL;
				set_state(STATE_PLAYER_CONTROL);
			}
			return true;

		case STATE_ROTATING:
			if (++state_tics_ == ROTATION_TICS) {
				if (++rotation_ == NUM_ROTATIONS)
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
	if (col_ == 0 && rotation_ == 1) {
		if (can_move(g, 0, 1)) {
			++col_;
			set_state(STATE_ROTATING);
		}

		return false;
	} else if (col_ == GRID_COLS - 1 && rotation_ == 3) {
		if (can_move(g, -1, 0)) {
			--col_;
			set_state(STATE_ROTATING);
		}

		return false;
	} else {
		int next_rotation = rotation_ + 1;
		if (next_rotation == NUM_ROTATIONS)
			next_rotation = 0;

		if (g->is_empty(row_ + offsets[next_rotation][0], col_ + offsets[next_rotation][1])) {
			set_state(STATE_ROTATING);
			return true;
		}

		return false;
	}
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
