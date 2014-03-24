#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "font.h"

class font_draw_strategy
{
public:
	font_draw_strategy(const font::glyph *const *glyphs)
	: glyphs_(glyphs)
	{ }

	virtual ~font_draw_strategy() { }

	virtual void operator()(gfx::context& gfx, int x, int y, const char *str) const = 0;

protected:
	const font::glyph *const *glyphs_;
};

class font_8x8_draw_strategy : public font_draw_strategy
{
public:
	font_8x8_draw_strategy(const font::glyph *const *glyphs)
	: font_draw_strategy(glyphs)
	{ }

	void operator()(gfx::context& gfx, int x, int y, const char *str) const;
};

class font_16x16_draw_strategy : public font_draw_strategy
{
public:
	font_16x16_draw_strategy(const font::glyph *const *glyphs)
	: font_draw_strategy(glyphs)
	{ }

	void operator()(gfx::context& gfx, int x, int y, const char *str) const;
};

class font_generic_draw_strategy : public font_draw_strategy
{
public:
	font_generic_draw_strategy(const font::glyph *const *glyphs, size_t glyph_width, size_t glyph_height)
	: font_draw_strategy(glyphs)
	, glyph_width_(glyph_width)
	, glyph_height_(glyph_height)
	{ }

	void operator()(gfx::context& gfx, int x, int y, const char *str) const;

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

	if (glyph_width == 8 && glyph_height == 8)
		draw_strategy_ = new font_8x8_draw_strategy(glyph_map_);
	else if (glyph_width == 16 && glyph_height == 16)
		draw_strategy_ = new font_16x16_draw_strategy(glyph_map_);
	else
		draw_strategy_ = new font_generic_draw_strategy(glyph_map_, glyph_width, glyph_height);
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
	(*draw_strategy_)(gfx, x, y, buf);
}

void
font_8x8_draw_strategy::operator()(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite_8x8(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
		x += 8;
	}
}

void
font_16x16_draw_strategy::operator()(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite_16x16(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
		x += 16;
	}
}

void
font_generic_draw_strategy::operator()(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite(x, y, g->u_, g->v_, glyph_width_, glyph_height_, gfx::rgb(255, 255, 255));
		x += glyph_width_;
	}
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
		p = new font(make_path(name, "TGA"), 8, 8);
		dict_.put(name, p);
	}

	return p;
}
