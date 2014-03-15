#ifndef GFX_RGB_H_
#define GFX_RGB_H_

#include <stdint.h>

namespace gfx {

struct rgb
{
	rgb(int red, int green, int blue)
	: red(red), green(green), blue(blue)
	{ }

	operator uint32_t() const
	{
		return (blue << 16) | (green << 8) | red;
	}

	int red, green, blue;
};

}

#endif // GFX_RGB_H_
