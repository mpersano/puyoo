#ifndef GFX_TEXTURE_H_
#define GFX_TEXTURE_H_

#include "image.h"

namespace gfx {

template <typename Impl>
class texture_base
{
public:
	texture_base(const image& img)
	: width_(img.width()), height_(img.height())
	{ }

	virtual ~texture_base()
	{ }

	void upload_to_vram() const
	{
		static_cast<const Impl *>(this)->upload_to_vram();
	}

	size_t width() const
	{ return width_; }

	size_t height() const
	{ return height_; }

	static texture_base *load_from_tga(const char *path)
	{
		gfx::image *img = gfx::image::load_from_tga(path);
		texture_base *tex = new Impl(*img);
		delete img;
		return tex;
	}

protected:
	size_t width_;
	size_t height_;
};

}

#include "texture_impl.h"

namespace gfx {
typedef texture_base<texture_impl> texture;
}

#endif // GFX_TEXTURE_H_
