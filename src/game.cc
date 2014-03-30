#include "game_state.h"
#include "in_game.h"
#include "common.h"
#include "grid.h"
#include "combo_animation.h"
#include "game.h"

static game_state *cur_state_;

void
game_initialize()
{
	grid_init_resources();
	combo_animation_init_resources();

	cur_state_ = new in_game;
}

void
game_redraw()
{
	static gfx::context_impl gfx;

	gfx.begin();
	cur_state_->draw(gfx);
	gfx.flush();
}

void
game_update()
{
	cur_state_->update(dpad_state);
}

void
game_release()
{
}
