#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <types.h>
#include <stdint.h>

class image;

namespace psx { namespace gpu {

class texture
{
public:
	virtual ~texture();

	void upload_to_vram() const;

	int texture_page() const
	{ return page_; }

	size_t width() const
	{ return width_; }

	size_t height() const
	{ return height_; }

	static texture *load_from_tga(const char *source, int texture_page);

private:
	texture(const image *img, int page);

	size_t width_;
	size_t height_;
	uint16_t *data_;
	int page_;
};

} }

#endif // TEXTURE_H_
