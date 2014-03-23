#include <stdio.h>

#include "common.h"
#include "file_reader.h"
#include "sprite.h"

class sprite_8x8 : public sprite
{
public:
	sprite_8x8(int u, int v)
	: sprite(u, v)
	{ }

	int width() const
	{ return 8; }

	int height() const
	{ return 8; }

	void draw(gfx::context& gfx, int x, int y) const
	{ gfx.add_sprite_8x8(x, y, u_, v_, gfx::rgb(255, 255, 255)); }
};

class sprite_16x16 : public sprite
{
public:
	sprite_16x16(int u, int v)
	: sprite(u, v)
	{ }

	int width() const
	{ return 16; }

	int height() const
	{ return 16; }

	void draw(gfx::context& gfx, int x, int y) const
	{ gfx.add_sprite_16x16(x, y, u_, v_, gfx::rgb(255, 255, 255)); }
};

class sprite_generic : public sprite
{
public:
	sprite_generic(int u, int v, int width, int height)
	: sprite(u, v)
	, width_(width)
	, height_(height)
	{ }

	int width() const
	{ return width_; }

	int height() const
	{ return height_; }

	void draw(gfx::context& gfx, int x, int y) const
	{ gfx.add_sprite(x, y, u_, v_, width_, height_, gfx::rgb(255, 255, 255)); }

private:
	int width_, height_;
};

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

		sprite *spr;

		if (width == 8 && height == 8)
			spr = new sprite_8x8(u, v);
		else if (width == 16 && height == 16)
			spr = new sprite_16x16(u, v);
		else
			spr = new sprite_generic(u, v, width, height);

		dict_.put(orig_image, spr);

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
