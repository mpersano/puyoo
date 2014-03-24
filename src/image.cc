#include <stdio.h>

#include "panic.h"
#include "file_reader.h"
#include "image.h"

enum {
	TGA_UNCOMPRESSED_TRUECOLOR = 2,
};

struct tga_cmap_info {
	uint16_t first_entry_index;
	uint16_t length;
	uint8_t entry_size;
} __attribute__ ((packed));

struct tga_image_spec {
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t pixel_depth;
	uint8_t image_descriptor;
} __attribute__ ((packed));

struct tga_header {
	uint8_t id_length;
	uint8_t cmap_type;
	uint8_t image_type;
	tga_cmap_info cmap_info;
	tga_image_spec image_spec;
} __attribute__ ((packed));

image *
image::load_from_tga(const char *path)
{
	file_reader reader(path);

	tga_header header;
	reader.read(&header, sizeof(header));

	const bool flipped = (header.image_spec.image_descriptor & 0x20) == 0;

	printf("width=%d height=%d depth=%d type=%d flipped=%d\n",
		header.image_spec.width, header.image_spec.height,
		header.image_spec.pixel_depth,
		header.image_type,
		flipped);

	if (header.image_type != TGA_UNCOMPRESSED_TRUECOLOR || header.image_spec.pixel_depth != 32) {
		panic("invalid type or pixel depth");
		return 0;
	}

	image *img = new image(header.image_spec.width, header.image_spec.height);

	for (size_t i = 0; i < img->height_; i++) {
		uint32_t *dest = &img->data_[(flipped ? img->height_ - 1 - i : i)*img->width_];
		reader.read(dest, img->width_*sizeof *dest);
	}

	return img;
}

image::image(size_t width, size_t height)
: width_(width)
, height_(height)
, data_(new uint32_t[width*height])
{ }

image::~image()
{
	delete[] data_;
}
