#ifndef GFX_CONTEXT_IMPL_H_
#define GFX_CONTEXT_IMPL_H_

#include "draw_list.h"

namespace gfx {

class psx_context : public context_base<psx_context>
{
public:
	void begin()
	{
		draw_list_.reset();
	}

	void flush()
	{
		draw_list_.draw();
	}

	void add_rectangle(int x, int y, int width, int height, const rgb& color)
	{
		draw_list_.add_rectangle(x, y, width, height, color);
	}

	void add_sprite_8x8(int x, int y, int u, int v, const rgb& color)
	{
		draw_list_.add_sprite_8x8(x, y, u, v, color);
	}

	void add_sprite_16x16(int x, int y, int u, int v, const rgb& color)
	{
		draw_list_.add_sprite_16x16(x, y, u, v, color);
	}

	void add_sprite(int x, int y, int u, int v, int width, int height, const rgb& color)
	{
		draw_list_.add_sprite(x, y, u, v, width, height, color);
	}

	void bind_texture(const texture *tex)
	{
		draw_list_.add_set_draw_mode(
			static_cast<const psx_texture *>(tex)->page(),
			psx::gpu::COLOR_MODE_15BIT_DIRECT);
	}

private:
	psx::gpu::draw_list draw_list_;
};

typedef psx_context context_impl;

}

#endif // GFX_CONTEXT_IMPL_H_
