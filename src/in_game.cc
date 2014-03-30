#include "common.h"
#include "gfx_context.h"
#include "in_game.h"

static const int grid_height = GRID_ROWS*BLOCK_SIZE;
static const int base_y = (SCREEN_HEIGHT - grid_height)/2;

static const int grid_width = GRID_COLS*BLOCK_SIZE;
static const int left_base_x = SCREEN_WIDTH/4 - grid_width/2;
static const int right_base_x = SCREEN_WIDTH*3/4 - grid_width/2;

in_game::in_game()
: left_grid_(left_base_x, base_y, &right_grid_, true)
, right_grid_(right_base_x, base_y, &left_grid_, false)
{ }

void
in_game::draw(gfx::context& gfx) const
{
	left_grid_.draw(gfx);
	right_grid_.draw(gfx);
}

void
in_game::update(unsigned dpad_state)
{
	left_grid_.update(dpad_state);
	right_grid_.update(dpad_state);
}
