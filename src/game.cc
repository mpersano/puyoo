#include <stdio.h>

#include "common.h"
#include "grid.h"
#include "game.h"
#include "gfx_context.h"

static grid left_grid, right_grid;

void
game_initialize()
{
printf("game_initialize\n");
	const int grid_height = GRID_ROWS*BLOCK_SIZE;
	const int grid_width = GRID_COLS*BLOCK_SIZE;
	const int base_y = (SCREEN_HEIGHT - grid_height)/2;

	left_grid.initialize(SCREEN_WIDTH/4 - grid_width/2, base_y, true);
	right_grid.initialize(SCREEN_WIDTH*3/4 - grid_width/2, base_y, false);

	grid_load_sprites();
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
	right_grid.update(0);
}

void
game_release()
{
}
