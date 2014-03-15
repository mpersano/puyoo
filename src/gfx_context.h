#ifndef GFX_CONTEXT_H_
#define GFX_CONTEXT_H_

#include "rgb.h"
#include "texture.h"

namespace gfx {

template <typename Impl>
class context_base
{
public:
	void begin()
	{
		static_cast<Impl *>(this)->begin();
	}

	void flush()
	{
		static_cast<Impl *>(this)->flush();
	}

	void add_rectangle(int x, int y, int width, int height, const rgb& color)
	{
		static_cast<Impl *>(this)->add_rectangle(x, y, width, height, color);
	}

	void add_sprite(int x, int y, int u, int v, int width, int height, const rgb& color)
	{
		static_cast<Impl *>(this)->add_sprite(x, y, u, v, width, height, color);
	}

	void bind_texture(const texture *tex)
	{
		static_cast<Impl *>(this)->bind_texture(tex);
	}
};

}

#include "gfx_context_impl.h"

namespace gfx {
typedef context_base<context_impl> context;
}

#endif // GFX_CONTEXT_H_
