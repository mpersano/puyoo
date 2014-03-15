#ifndef GFX_CONTEXT_H_
#define GFX_CONTEXT_H_

#include "rgb.h"

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
};

}

#include "gfx_context_impl.h"

namespace gfx {
typedef context_base<context_impl> context;
}

#endif // GFX_CONTEXT_H_
