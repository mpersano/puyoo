#ifndef FONT_H_
#define FONT_H_

#include "dict.h"
#include "texture.h"
#include "gfx_context.h"

class font
{
public:
	font(const char *texture_path, size_t glyph_width, size_t glyph_height);
	virtual ~font();

	void draw(gfx::context& gfx, int x, int y, const char *fmt, ...) const;

	struct glyph {
		glyph(int u, int v) : u_(u), v_(v) { }
		int u_, v_;
	};

	virtual void draw_glyph(gfx::context& gfx, int x, int y, const glyph *g) const = 0;

	virtual size_t glyph_width() const = 0;

protected:
	gfx::texture *texture_;
	glyph *glyph_map_[256];
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
