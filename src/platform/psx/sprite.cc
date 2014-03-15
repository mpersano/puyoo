#include <stdio.h>

#include "draw_list.h"
#include "sprite.h"

void
sprite::draw(psx::gpu::draw_list& draw_list, int x, int y) const
{
	draw_list.add_sprite(
		x, y,
		u_, v_,
		width_, height_,
		psx::gpu::rgb(255, 255, 255));
}

class file_reader
{
public:
	file_reader(const char *path);
	virtual ~file_reader();

	uint8_t read_uint8();
	uint16_t read_uint16();
	uint32_t read_uint32();
	char *read_string();

private:
	FILE *fd_;;
};

file_reader::file_reader(const char *path)
: fd_(fopen(path, "rb"))
{ }

file_reader::~file_reader()
{
	fclose(fd_);
}

uint8_t
file_reader::read_uint8()
{
	uint8_t value;
	fread(&value, 1, 1, fd_);
	return value;
}

uint16_t
file_reader::read_uint16()
{
	uint16_t lo = static_cast<uint16_t>(read_uint8());
	uint16_t hi = static_cast<uint16_t>(read_uint8());
	return lo | (hi << 8);
}

uint32_t
file_reader::read_uint32()
{
	uint32_t lo = static_cast<uint32_t>(read_uint16());
	uint32_t hi = static_cast<uint32_t>(read_uint16());
	return lo | (hi << 16);
}

char *
file_reader::read_string()
{
	uint8_t len = read_uint8();

	char *str = new char[len + 1];
	fread(str, 1, len, fd_);
	str[len] = '\0';

	return str;
}

sprite_atlas::sprite_atlas(const char *name)
{
	char texture_path[80];
	sprintf(texture_path, "cdrom:\\%s.TGA;1", name);

	texture_ = gfx::texture::load_from_tga(texture_path);
	// XXX: error checking

	char spr_path[80];
	sprintf(spr_path, "cdrom:\\%s.SPR;1", name);

	file_reader reader(spr_path);

	int num_sprites = reader.read_uint8();

	for (int i = 0; i < num_sprites; i++) {
		char *orig_image = reader.read_string();
		char *sheet_image = reader.read_string();

		int u = reader.read_uint16();
		int v = reader.read_uint16();
		int width = reader.read_uint16();
		int height = reader.read_uint16();

		printf("sprite: %s %s %d %d %d %d\n",
		  orig_image, sheet_image, u, v, width, height);

		dict_.put(orig_image, new sprite(u, v, width, height));

		delete[] orig_image;
		delete[] sheet_image;
	}
}

sprite_atlas::~sprite_atlas()
{
	delete texture_;
}

sprite_atlas_manager&
sprite_atlas_manager::instance()
{
	static sprite_atlas_manager instance_;
	return instance_;
}

sprite_atlas_manager::sprite_atlas_manager()
{ }

sprite_atlas *
sprite_atlas_manager::get(const char *name)
{
	sprite_atlas *p = dict_.get(name);

	if (!p) {
		p = new sprite_atlas(name);
		dict_.put(name, p);
	}

	return p;
}
