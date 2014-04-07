#ifndef SPRITE_H_
#define SPRITE_H_

#include "dict.h"
#include "texture.h"
#include "gfx_context.h"

struct rect
{
	rect(int x_min, int x_max, int y_min, int y_max)
	: x_min_(x_min), x_max_(x_max)
	, y_min_(y_min), y_max_(y_max)
	{ }

	int x_min_, x_max_;
	int y_min_, y_max_;
};

struct sprite
{
public:
	sprite(int u, int v)
	: u_(u), v_(v)
	{ }

	virtual ~sprite()
	{ }

	int u() const
	{ return u_; }

	int v() const
	{ return v_; }

	virtual int width() const = 0;

	virtual int height() const = 0;

	virtual void draw(gfx::context& gfx, int x, int y) const = 0;

	void draw(gfx::context& gfx, const rect& clip, int x, int y) const;

	static sprite *make(int u, int v, int width, int height);

protected:
	int u_, v_;
};

class sprite_atlas
{
public:
	sprite_atlas(const char *name);
	virtual ~sprite_atlas();

	const sprite *get_sprite(const char *name) const
	{ return dict_.get(name); }

	const gfx::texture *get_texture() const
	{ return texture_; }

private:
	gfx::texture *texture_;
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
