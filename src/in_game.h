#ifndef IN_GAME_H_
#define IN_GAME_H_

#include "game_state.h"
#include "grid.h"

class in_game : public game_state
{
public:
	in_game();

	void draw(gfx::context& gfx) const;
	void update(unsigned dpad_state);

private:
	grid left_grid_, right_grid_;
};

#endif // IN_GAME_H_
