#ifndef IMAGE_H_
#define IMAGE_H_

#include <types.h>
#include <stdint.h>

class image
{
public:
	virtual ~image();

	size_t width() const
	{ return width_; }

	size_t height() const
	{ return height_; }

	const uint32_t *data() const
	{ return data_; }

	static image *load_from_tga(const char *path);

private:
	image(size_t width, size_t height);

	size_t width_;
	size_t height_;
	uint32_t *data_;
};

#endif // IMAGE_H_
