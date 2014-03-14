#include <psx.h>

#include "image.h"
#include "texture.h"

namespace psx { namespace gpu {

texture::texture(int width, int height, uint16_t *data, int page)
: width_(width)
, height_(height)
, data_(data)
, page_(page)
{ }

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

	// convert to 15-bit

	uint16_t *data = new uint16_t[img->width()*img->height()];

	const uint32_t *src = img->data();

	for (size_t i = 0; i < img->height(); i++) {
		uint16_t *dest = &data[(img->height() - 1 - i)*img->width()];

		for (size_t j = 0; j < img->width(); j++) {
			uint32_t v = *src++;
			int r = v & 0xff;
			int g = (v >> 8) & 0xff;
			int b = (v >> 16) & 0xff;
			int a = (v >> 24);
			*dest++ = a == 0 ? 0 : (b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10) | 0x8000;
		}
	}

	texture *tex = new texture(img->width(), img->height(), data, texture_page);

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
