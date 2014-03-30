#ifndef COMBO_ANIMATION_H_
#define COMBO_ANIMATION_H_

#include "gfx_context.h"

class combo_animation
{
public:
	combo_animation();

	void reset(int combo_size);
	void draw(gfx::context& gfx, int base_x, int base_y) const;
	void update();

protected:
	bool is_active_;
	int tics_;
	int combo_size_;
};

void
combo_animation_init_resources();

#endif // COMBO_ANIMATION_H_
