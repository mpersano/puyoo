#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <stdint.h>

namespace psx { namespace gpu {

class texture
{
public:
	virtual ~texture();

	void upload_to_vram() const;

	int texture_page() const
	{ return page_; }

	int width() const
	{ return width_; }

	int height() const
	{ return height_; }

	static texture *load_from_tga(const char *source, int texture_page);

private:
	texture(int width, int height, uint16_t *data, int page);

	int width_;
	int height_;
	uint16_t *data_;
	int page_;
};

} }

#endif // TEXTURE_H_
