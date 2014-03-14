#ifndef GFX_CONTEXT_IMPL_H_
#define GFX_CONTEXT_IMPL_H_

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

private:
	psx::gpu::draw_list draw_list_;
};

}

#endif // GFX_CONTEXT_IMPL_H_
