#ifndef SPRITE_H_
#define SPRITE_H_

#include "dict.h"

namespace psx { namespace gpu {
class texture;
} }

struct sprite
{
public:
	sprite(int u, int v, int width, int height)
	: u_(u), v_(v)
	, width_(width), height_(height)
	{ }

	int u() const
	{ return u_; }

	int v() const
	{ return v_; }

	int width() const
	{ return width_; }

	int height() const
	{ return height_; }

private:
	int u_, v_;
	int width_, height_;
};

class sprite_atlas
{
public:
	sprite_atlas(const char *name);
	virtual ~sprite_atlas();

	const sprite *get_sprite(const char *name)
	{ return dict_.get(name); }

	const psx::gpu::texture *get_texture() const
	{ return texture_; }

private:
	psx::gpu::texture *texture_;
	dict<sprite> dict_;
};

class sprite_atlas_manager
{
public:
	static sprite_atlas_manager& instance();

	sprite_atlas *get(const char *name);

private:
	sprite_atlas_manager();
	dict<sprite_atlas> dict_;
};

#endif // SPRITE_H_
