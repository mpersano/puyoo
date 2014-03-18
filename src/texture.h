#ifndef GFX_TEXTURE_H_
#define GFX_TEXTURE_H_

#include "image.h"

namespace gfx {

template <class Impl>
class texture_base
{
public:
	virtual ~texture_base()
	{ delete image_; }

	static texture_base *load_from_tga(const char *path)
	{
		image *img = image::load_from_tga(path);
		return img ? new Impl(img) : 0;
	}

	void upload_to_vram() const
	{
		static_cast<const Impl *>(this)->upload_to_vram();
	}

	size_t width() const
	{ return image_->width(); }

	size_t height() const
	{ return image_->height(); }

protected:
	texture_base(image *img)
	: image_(img)
	{ }

	image *image_;
};

}

#include "texture_impl.h"

namespace gfx {
typedef texture_base<texture_impl> texture;
}

#endif // GFX_TEXTURE_H_
