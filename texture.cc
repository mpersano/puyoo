#include <psx.h>

#include "image.h"
#include "texture.h"

namespace psx { namespace gpu {

texture::texture(const image *img, int page)
: width_(img->width())
, height_(img->height())
, data_(new uint16_t[img->width()*img->height()])
, page_(page)
{
	const uint32_t *src = img->data();

	for (size_t i = 0; i < height_; i++) {
		uint16_t *dest = &data_[(height_ - 1 - i)*width_];

		for (size_t j = 0; j < width_; j++) {
			uint32_t v = *src++;
			int r = v & 0xff;
			int g = (v >> 8) & 0xff;
			int b = (v >> 16) & 0xff;
			int a = (v >> 24);
			*dest++ = a == 0 ? 0 : (b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10) | 0x8000;
		}
	}
}

texture::~texture()
{
	delete[] data_;
}

texture *
texture::load_from_tga(const char *source, int texture_page)
{
	image *img = image::load_from_tga(source);

	if (!img)
		return 0;

	texture *tex = new texture(img, texture_page);

	delete img;

	return tex;
}

void
texture::upload_to_vram() const
{
	int vram_x = (page_ & 0xf) << 6;
	int vram_y = ((page_ >> 4) & 1) << 8;

	LoadImage(data_, vram_x, vram_y, width_, height_);

	while (GsIsDrawing())
		;
}

} }
