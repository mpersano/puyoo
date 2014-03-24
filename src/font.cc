#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "font.h"

font::font(const char *texture_path, size_t glyph_width, size_t glyph_height)
: texture_(gfx::texture::load_from_tga(texture_path))
, glyph_width_(glyph_width)
, glyph_height_(glyph_height)
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
		glyph *g = &glyph_map_[static_cast<int>(*p)];

		g->valid_ = true;
		g->u_ = u;
		g->v_ = v;

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
		const glyph *g = &glyph_map_[static_cast<int>(*p)];
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
