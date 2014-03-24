#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "font.h"

class font_8x8 : public font
{
public:
	font_8x8(const char *texture_path);

	void draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const;

	size_t glyph_width() const
	{ return 8; }
};

class font_16x16 : public font
{
public:
	font_16x16(const char *texture_path);

	void draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const;

	size_t glyph_width() const
	{ return 16; }
};

class font_generic : public font
{
public:
	font_generic(const char *texture_path, size_t glyph_width, size_t glyph_height);

	void draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const;

	size_t glyph_width() const
	{ return glyph_width_; }

private:
	size_t glyph_width_, glyph_height_;
};

font::font(const char *texture_path, size_t glyph_width, size_t glyph_height)
: texture_(gfx::texture::load_from_tga(texture_path))
{
	texture_->upload_to_vram();

	memset(glyph_map_, 0, sizeof(glyph_map_));

	const char *chars =
		" !\"#$%&'"
		"()*+,-./"
		"01234567"
		"89:;<=>?"
		"@ABCDEFG"
		"HIJKLMNO"
		"PQRSTUVW"
		"XYZ[\\]^_"
		"`abcdefg"
		"hijklmno"
		"pqrstuvw"
		"xyz{|}~";

	size_t u = 0, v = 0;

	for (const char *p = chars; *p; p++) {
		glyph_map_[static_cast<int>(*p)] = new glyph(u, v);

		if ((u += glyph_width) >= texture_->width()) {
			u = 0;
			v += glyph_height;
		}
	}
}

font::~font()
{
	delete texture_;
}

void
font::draw(gfx::context& gfx, int x, int y, const char *fmt, ...) const
{
	char buf[128];

	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	gfx.bind_texture(texture_);

	for (const char *p = buf; *p; p++) {
		const glyph *g = glyph_map_[static_cast<int>(*p)];
		if (g)
			draw_glyph(gfx, x, y, g);
		x += glyph_width();
	}
}

font_8x8::font_8x8(const char *texture_path)
: font(texture_path, 8, 8)
{ }

void
font_8x8::draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const
{
	gfx.add_sprite_8x8(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
}

font_16x16::font_16x16(const char *texture_path)
: font(texture_path, 16, 16)
{ }

void
font_16x16::draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const
{
	gfx.add_sprite_16x16(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
}

font_generic::font_generic(const char *texture_path, size_t glyph_width, size_t glyph_height)
: font(texture_path, glyph_width, glyph_height)
, glyph_width_(glyph_width)
, glyph_height_(glyph_height)
{ }

void
font_generic::draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const
{
	gfx.add_sprite(x, y, g->u_, g->v_, glyph_width_, glyph_height_, gfx::rgb(255, 255, 255));
}

font_manager&
font_manager::instance()
{
	static font_manager instance_;
	return instance_;
}

font_manager::font_manager()
{ }

const font *
font_manager::get(const char *name)
{
	font *p = dict_.get(name);

	if (!p) {
		p = new font_8x8(make_path(name, "TGA"));
		dict_.put(name, p);
	}

	return p;
}
