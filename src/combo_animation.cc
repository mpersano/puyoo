#include <stdio.h>

#include "common.h"
#include "grid.h"
#include "sprite.h"
#include "combo_animation.h"

enum {
	ANIMATION_TICS = 80,
	MIN_COMBO_SIZE = 2,
	MAX_COMBO_SIZE = 5,
};

static const sprite *combo_sprite;
static const sprite *combo_size_sprites[MAX_COMBO_SIZE - MIN_COMBO_SIZE + 1];

void
combo_animation_init_resources()
{
	sprite_atlas *sprites = sprite_atlas_manager::instance().get("SPRITES");

	combo_sprite = sprites->get_sprite("combo.png");

	for (int i = MIN_COMBO_SIZE; i <= MAX_COMBO_SIZE; i++) {
		char name[80];
		sprintf(name, "%d.png", i);
		combo_size_sprites[i - MIN_COMBO_SIZE] = sprites->get_sprite(name);
	}
}

combo_animation::combo_animation()
: is_active_(false)
, tics_(0)
{ }

void
combo_animation::reset(int combo_size)
{
	if (combo_size >= MIN_COMBO_SIZE && combo_size <= MAX_COMBO_SIZE) {
		combo_size_ = combo_size - MIN_COMBO_SIZE;
		tics_ = 0;
		is_active_ = true;
	}
}

void
combo_animation::draw(gfx::context& gfx, int base_x, int base_y) const
{
	if (is_active_) {
		static const int offset[ANIMATION_TICS] = { 
			250, 239, 228, 218, 209, 200, 192, 184, 
			176, 169, 163, 157, 151, 146, 141, 136, 
			132, 128, 124, 121, 118, 116, 113, 111, 
			109, 107, 106, 105, 104, 103, 102, 101, 
			101, 100, 100, 100, 100, 100, 100, 100, 
			100, 99, 99, 99, 99, 99, 99, 99, 
			98, 98, 97, 96, 95, 94, 93, 92, 
			90, 88, 86, 83, 81, 78, 75, 71, 
			67, 63, 58, 53, 48, 42, 36, 30, 
			23, 15, 7, 0, -9, -18, -28, -39, };
		const rect clip_rect(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);

		const int y = base_y + offset[tics_];

		const sprite *size_sprite = combo_size_sprites[combo_size_];
		size_sprite->draw(gfx, clip_rect, base_x + 4, y - size_sprite->height()/2);
		combo_sprite->draw(gfx, clip_rect, base_x + 24, y - combo_sprite->height()/2);
	}
}

void
combo_animation::update()
{
	if (is_active_) {
		if (++tics_ == ANIMATION_TICS)
			is_active_ = false;
	}
}
