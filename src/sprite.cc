#include <stdio.h>

#include "common.h"
#include "file_reader.h"
#include "sprite.h"

void
sprite::draw(gfx::context& gfx, int x, int y) const
{
	if (width_ == 8 && height_ == 8)
		gfx.add_sprite_8x8(x, y, u_, v_, gfx::rgb(255, 255, 255));
	else if (width_ == 16 && height_ == 16)
		gfx.add_sprite_16x16(x, y, u_, v_, gfx::rgb(255, 255, 255));
	else
		gfx.add_sprite(x, y, u_, v_, width_, height_, gfx::rgb(255, 255, 255));
}

sprite_atlas::sprite_atlas(const char *name)
{
	texture_ = gfx::texture::load_from_tga(make_path(name, "TGA"));
	// XXX: error checking

	file_reader reader(make_path(name, "SPR"));

	int num_sprites = reader.read_uint8();

	for (int i = 0; i < num_sprites; i++) {
		char *orig_image = reader.read_string();
		char *sheet_image = reader.read_string();

		int u = reader.read_uint16();
		int v = reader.read_uint16();
		int width = reader.read_uint16();
		int height = reader.read_uint16();

		printf("sprite: %s %s %d %d %d %d\n",
		  orig_image, sheet_image, u, v, width, height);

		dict_.put(orig_image, new sprite(u, v, width, height));

		delete[] orig_image;
		delete[] sheet_image;
	}
}

sprite_atlas::~sprite_atlas()
{
	delete texture_;
}

sprite_atlas_manager&
sprite_atlas_manager::instance()
{
	static sprite_atlas_manager instance_;
	return instance_;
}

sprite_atlas_manager::sprite_atlas_manager()
{ }

sprite_atlas *
sprite_atlas_manager::get(const char *name)
{
	sprite_atlas *p = dict_.get(name);

	if (!p) {
		p = new sprite_atlas(name);
		dict_.put(name, p);
	}

	return p;
}
