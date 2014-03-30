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
		static const int offset[ANIMATION_TICS] = { 250, 242, 235, 228, 221, 214, 208, 202, 196, 190, 184, 178, 173, 168, 163, 158, 154, 149, 145, 141, 137, 133, 130, 127, 124, 121, 118, 115, 113, 111, 109, 107, 106, 104, 103, 102, 101, 100, 100, 100, 100, 99, 99, 99, 98, 97, 96, 95, 94, 92, 90, 88, 86, 84, 81, 78, 76, 72, 69, 66, 62, 58, 54, 50, 45, 41, 36, 31, 26, 21, 15, 9, 3, -2, -8, -14, -21, -28, -35, -42, };
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
