#ifndef GFX_TEXTURE_IMPL_H_
#define GFX_TEXTURE_IMPL_H_

#include <psx.h>

namespace gfx {

extern int get_texture_page(int width, int height);

class psx_texture : public texture_base<psx_texture>
{
public:
	psx_texture(const image& img)
	: texture_base<psx_texture>(img)
	, page_(get_texture_page(width_, height_))
	{
		data_ = new uint16_t[width_*height_];

		const uint32_t *src = img.data();
		uint16_t *dest = data_;

		for (size_t j = 0; j < width_*height_; j++) {
			uint32_t v = *src++;
			int r = v & 0xff;
			int g = (v >> 8) & 0xff;
			int b = (v >> 16) & 0xff;
			int a = (v >> 24);
			*dest++ = a == 0 ? 0 : (b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10) | 0x8000;
		}
	}

	~psx_texture()
	{
		delete[] data_;
	}

	void upload_to_vram() const
	{
		int vram_x = (page_ & 0xf) << 6;
		int vram_y = ((page_ >> 4) & 1) << 8;

		LoadImage(data_, vram_x, vram_y, width_, height_);
		while (GsIsDrawing())
			;
	}

	int page() const
	{ return page_; }

private:
	int page_;
	uint16_t *data_;
};

typedef psx_texture texture_impl;

}

#endif // GFX_TEXTURE_IMPL_H_
