#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "file_reader.h"
#include "font.h"

class font_renderer
{
public:
	font_renderer(const font::glyph *const *glyphs)
	: glyphs_(glyphs)
	{ }

	virtual ~font_renderer() { }

	virtual void render(gfx::context& gfx, int x, int y, const char *str) const = 0;

	static font_renderer *make(const font::glyph *const *glyphs, int glyph_width, int glyph_height);

protected:
	const font::glyph *const *glyphs_;
};

class font_8x8_renderer : public font_renderer
{
public:
	font_8x8_renderer(const font::glyph *const *glyphs)
	: font_renderer(glyphs)
	{ }

	void render(gfx::context& gfx, int x, int y, const char *str) const;
};

class font_16x16_renderer : public font_renderer
{
public:
	font_16x16_renderer(const font::glyph *const *glyphs)
	: font_renderer(glyphs)
	{ }

	void render(gfx::context& gfx, int x, int y, const char *str) const;
};

class font_generic_renderer : public font_renderer
{
public:
	font_generic_renderer(const font::glyph *const *glyphs, size_t glyph_width, size_t glyph_height)
	: font_renderer(glyphs)
	, glyph_width_(glyph_width)
	, glyph_height_(glyph_height)
	{ }

	void render(gfx::context& gfx, int x, int y, const char *str) const;

private:
	size_t glyph_width_, glyph_height_;
};

font::font(const char *name)
{
	load_texture(name);
	load_glyphs(name);
}

void
font::load_texture(const char *name)
{
	gfx::image *img = gfx::image::load_from_tga(make_path(name, "TGA"));

	texture_ = new gfx::texture_impl(*img);
	texture_->upload_to_vram();

	delete img;
}

void
font::load_glyphs(const char *name)
{
	file_reader reader(make_path(name, "FNT"));

	const int glyph_width = reader.read_uint8();
	const int glyph_height = reader.read_uint8();

	char *chars = reader.read_string();

	memset(glyph_map_, 0, sizeof(glyph_map_));

	size_t u = 0, v = 0;

	for (const char *p = chars; *p; p++) {
		glyph_map_[static_cast<int>(*p)] = new glyph(u, v);

		if ((u += glyph_width) >= texture_->width()) {
			u = 0;
			v += glyph_height;
		}
	}

	delete[] chars;

	renderer_ = font_renderer::make(glyph_map_, glyph_width, glyph_height);
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

	gfx.bind_texture(*texture_);
	renderer_->render(gfx, x, y, buf);
}

void
font_8x8_renderer::render(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite_8x8(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
		x += 8;
	}
}

void
font_16x16_renderer::render(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite_16x16(x, y, g->u_, g->v_, gfx::rgb(255, 255, 255));
		x += 16;
	}
}

void
font_generic_renderer::render(gfx::context& gfx, int x, int y, const char *str) const
{
	for (const char *p = str; *p; p++) {
		const font::glyph *g = glyphs_[static_cast<int>(*p)];
		gfx.add_sprite(x, y, g->u_, g->v_, glyph_width_, glyph_height_, gfx::rgb(255, 255, 255));
		x += glyph_width_;
	}
}

font_renderer *
font_renderer::make(const font::glyph *const *glyphs, int glyph_width, int glyph_height)
{
	if (glyph_width == 8 && glyph_height == 8)
		return new font_8x8_renderer(glyphs);
	else if (glyph_width == 16 && glyph_height == 16)
		return new font_16x16_renderer(glyphs);
	else
		return new font_generic_renderer(glyphs, glyph_width, glyph_height);
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
		p = new font(name);
		dict_.put(name, p);
	}

	return p;
}
