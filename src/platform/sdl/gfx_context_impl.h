#ifndef GFX_CONTEXT_IMPL_H_
#define GFX_CONTEXT_IMPL_H_

namespace gfx {

class gl_context : public context_base<gl_context>
{
public:
	void begin()
	{ }

	void flush()
	{ }

	void add_rectangle(int x, int y, int width, int height, const rgb& color)
	{
		glDisable(GL_TEXTURE_RECTANGLE);

		glColor4ub(color.red, color.green, color.blue, 255);

		glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x + width, y);
		glVertex2i(x + width, y + height);
		glVertex2i(x, y + height);
		glEnd();
	}

	void add_sprite(int x, int y, int u, int v, int width, int height, const rgb& color)
	{
		glColor4ub(color.red, color.green, color.blue, 255);

		glBegin(GL_QUADS);
		glTexCoord2i(u, v);
		glVertex2i(x, y);
		glTexCoord2i(u + width, v);
		glVertex2i(x + width, y);
		glTexCoord2i(u + width, v + height);
		glVertex2i(x + width, y + height);
		glTexCoord2i(u, v + height);
		glVertex2i(x, y + height);
		glEnd();
	}

	void bind_texture(const texture *tex)
	{
		glEnable(GL_TEXTURE_RECTANGLE);
		static_cast<const texture_impl *>(tex)->bind();
	}
};

typedef gl_context context_impl;

}

#endif // GFX_CONTEXT_IMPL_H_
