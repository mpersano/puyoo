#ifndef FONT_H_
#define FONT_H_

#include "dict.h"
#include "texture.h"
#include "gfx_context.h"

class font_renderer;

class font
{
public:
	font(const char *name);
	virtual ~font();

	void draw(gfx::context& gfx, int x, int y, const char *fmt, ...) const;

	struct glyph {
		glyph(int u, int v) : u_(u), v_(v) { }
		int u_, v_;
	};

protected:
	void load_texture(const char *name);
	void load_glyphs(const char *name);

	gfx::texture_impl texture_;
	glyph *glyph_map_[256];
	font_renderer *renderer_;
};

class font_manager
{
public:
	static font_manager& instance();

	const font *get(const char *name);

private:
	font_manager();

	dict<font> dict_;
};

#endif // FONT_H_
