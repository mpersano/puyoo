#include <stdio.h>

#include "common.h"
#include "file_reader.h"
#include "sprite.h"

void
sprite::draw(gfx::context& gfx, const rect& clip, int x, int y) const
{
	int x0 = x, x1 = x + width();
	int u = u_;

	if (x0 < clip.x_min_) {
		if (x1 <= clip.x_min_)
			return;
		u += clip.x_min_ - x0;
		x0 = clip.x_min_;
	}

	if (x1 > clip.x_max_) {
		if (x0 >= clip.x_max_)
			return;
		x1 = clip.x_max_;
	}

	int y0 = y, y1 = y + height();
	int v = v_;

	if (y0 < clip.y_min_) {
		if (y1 <= clip.y_min_)
			return;
		v += clip.y_min_ - y0;
		y0 = clip.y_min_;
	}

	if (y1 > clip.y_max_) {
		if (y0 >= clip.y_max_)
			return;
		y1 = clip.y_max_;
	}

	gfx.add_sprite(x0, y0, u, v, x1 - x0, y1 - y0, gfx::rgb(255, 255, 255));
}

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
	load_texture(name);
	load_sprites(name);
}

sprite_atlas::~sprite_atlas()
{
	delete texture_;
}

void
sprite_atlas::load_texture(const char *name)
{
	gfx::image *img = gfx::image::load_from_tga(make_path(name, "TGA"));

	texture_ = new gfx::texture_impl(*img);
	texture_->upload_to_vram();

	delete img;
}

void
sprite_atlas::load_sprites(const char *name)
{
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
