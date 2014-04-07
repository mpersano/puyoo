#ifndef GFX_TEXTURE_IMPL_H_
#define GFX_TEXTURE_IMPL_H_

#include <GL/gl.h>
#include <string.h>

namespace gfx {

class gl_texture : public texture_base<gl_texture>
{
public:
	gl_texture(const image& img)
	: texture_base<gl_texture>(img)
	, data_(new uint32_t[width_*height_])
	{
		glGenTextures(1, &id_);

		const uint32_t *src = img.data();
		uint32_t *dest = data_;

		for (size_t i = 0; i < width_*height_; i++) {
			uint32_t v = *src++;
			*dest++ = ((v >> 16) & 0xff) | (v & 0xff00) | ((v & 0xff) << 16) | (v & 0xff000000);
		}
	}

	~gl_texture()
	{
		delete[] data_;

		glDeleteTextures(1, &id_);
	}

	void upload_to_vram() const
	{
		bind();

		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_RGBA,
			width_, height_,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data_);
	}

	void bind() const
	{
		glBindTexture(GL_TEXTURE_RECTANGLE, id_);
	}

private:
	GLuint id_;
	uint32_t *data_;
};

typedef gl_texture texture_impl;

}

#endif // GFX_TEXTURE_IMPL_H_
