#ifndef GFX_TEXTURE_H_
#define GFX_TEXTURE_H_

#include "image.h"

namespace gfx {

template <typename Impl>
class texture_base
{
public:
	texture_base()
	: width_(0), height_(0)
	{ }

	virtual ~texture_base()
	{ }

	void set_data(const image& img)
	{
		static_cast<Impl *>(this)->set_data(img);
	}

	void upload_to_vram() const
	{
		static_cast<const Impl *>(this)->upload_to_vram();
	}

	size_t width() const
	{ return width_; }

	size_t height() const
	{ return height_; }

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
