#ifndef GFX_TEXTURE_IMPL_H_
#define GFX_TEXTURE_IMPL_H_

#include <GL/gl.h>
#include <string.h>

namespace gfx {

class gl_texture : public texture_base<gl_texture>
{
public:
	gl_texture(image *img)
	: texture_base<gl_texture>(img) 
	{
		glGenTextures(1, &id_);
	}

	~gl_texture()
	{
		glDeleteTextures(1, &id_);
	}

	void upload_to_vram() const
	{
		uint32_t *data = new uint32_t[image_->width()*image_->height()];

		const uint32_t *src = image_->data();
		uint32_t *dest = data;

		for (size_t i = 0; i < image_->width()*image_->height(); i++) {
			uint32_t v = *src++;
			*dest++ = ((v >> 16) & 0xff) | (v & 0xff00) | ((v & 0xff) << 16) | (v & 0xff000000);
		}

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
			image_->width(), image_->height(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data);

		delete[] data;
	}

	void bind() const
	{
		glBindTexture(GL_TEXTURE_RECTANGLE, id_);
	}

private:
	GLuint id_;
};

typedef gl_texture texture_impl;

}

#endif // GFX_TEXTURE_IMPL_H_
