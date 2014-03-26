#include "common.h"
#include "gfx_context.h"
#include "in_game.h"

in_game::in_game()
{
	const int grid_height = GRID_ROWS*BLOCK_SIZE;
	const int grid_width = GRID_COLS*BLOCK_SIZE;
	const int base_y = (SCREEN_HEIGHT - grid_height)/2;

	left_grid.initialize(SCREEN_WIDTH/4 - grid_width/2, base_y, &right_grid, true);
	right_grid.initialize(SCREEN_WIDTH*3/4 - grid_width/2, base_y, &left_grid, false);
}

void
in_game::draw(gfx::context& gfx) const
{
	left_grid.draw(gfx);
	right_grid.draw(gfx);
}

void
in_game::update(unsigned dpad_state)
{
	left_grid.update(dpad_state);
	right_grid.update(dpad_state);
}
