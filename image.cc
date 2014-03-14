extern "C" {
#include <stdio.h>
}

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
image::load_from_tga(const char *source)
{
	FILE *in;
	
	if ((in = fopen(source, "rb")) == 0)
		return 0;

	tga_header header;

	if (fread(&header, sizeof(header), 1, in) != 1) {
		fclose(in);
		return 0;
	}

	printf("width=%d height=%d depth=%d type=%d\n", header.image_spec.width, header.image_spec.height, header.image_spec.pixel_depth, header.image_type);

	if (header.image_type != TGA_UNCOMPRESSED_TRUECOLOR || header.image_spec.pixel_depth != 32) {
		fclose(in);
		return 0;
	}

	image *img = new image(header.image_spec.width, header.image_spec.height);

	if (fread(img->data_, sizeof(uint32_t), img->width_*img->height_, in) != img->width_*img->height_) {
		delete img;
		fclose(in);
		return 0;
	}

	fclose(in);

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
