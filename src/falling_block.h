#ifndef FALLING_BLOCK_H_
#define FALLING_BLOCK_H_

#include "gfx_context.h"

class grid;
class control_strategy;

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

#endif // FALLING_BLOCK_H_
