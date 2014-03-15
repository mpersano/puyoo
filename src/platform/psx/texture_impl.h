#ifndef GFX_TEXTURE_IMPL_H_
#define GFX_TEXTURE_IMPL_H_

#include <psx.h>

namespace gfx {

class psx_texture : public texture_base<psx_texture>
{
public:
	psx_texture(image *img)
	: texture_base<psx_texture>(img)
	, page_(5)
	{ }

	void upload_to_vram() const
	{
		const size_t width = image_->width();
		const size_t height = image_->height();

		uint16_t *data = new uint16_t[width*height];

		const uint32_t *src = image_->data();

		for (size_t i = 0; i < height; i++) {
			uint16_t *dest = &data[(height - 1 - i)*width];

			for (size_t j = 0; j < width; j++) {
				uint32_t v = *src++;
				int r = v & 0xff;
				int g = (v >> 8) & 0xff;
				int b = (v >> 16) & 0xff;
				int a = (v >> 24);
				*dest++ = a == 0 ? 0 : (b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10) | 0x8000;
			}
		}

		int vram_x = (page_ & 0xf) << 6;
		int vram_y = ((page_ >> 4) & 1) << 8;

		LoadImage(data, vram_x, vram_y, width, height);
		while (GsIsDrawing())
			;

		delete[] data;
	}

	int page() const
	{ return page_; }

private:
	int page_;
};

typedef psx_texture texture_impl;
typedef texture_base<psx_texture> texture;

}

#endif // GFX_TEXTURE_IMPL_H_
