#include "common.h"
#include "grid.h"
#include "game.h"
#include "font.h"
#include "gfx_context.h"

static grid left_grid, right_grid;

void
game_initialize()
{
	grid_init_resources();

	const int grid_height = GRID_ROWS*BLOCK_SIZE;
	const int grid_width = GRID_COLS*BLOCK_SIZE;
	const int base_y = (SCREEN_HEIGHT - grid_height)/2;

	left_grid.initialize(SCREEN_WIDTH/4 - grid_width/2, base_y, &right_grid, true);
	right_grid.initialize(SCREEN_WIDTH*3/4 - grid_width/2, base_y, &left_grid, false);
}

void
game_redraw()
{
	static gfx::context_impl gfx;

	gfx.begin();
	left_grid.draw(gfx);
	right_grid.draw(gfx);
	gfx.flush();
}

void
game_update()
{
	left_grid.update(dpad_state);
	right_grid.update(dpad_state);
}

void
game_release()
{
}
