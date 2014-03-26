#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include "gfx_context.h"

class game_state
{
public:
	virtual ~game_state() { }

	virtual void draw(gfx::context& gfx) const = 0;
	virtual void update(unsigned dpad_state) = 0;
};

#endif // GAME_STATE_H_
