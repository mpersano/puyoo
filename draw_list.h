#ifndef DRAW_LIST_H_
#define DRAW_LIST_H_

#include <stdint.h>

namespace psx { namespace gpu {

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

enum color_mode {
	COLOR_MODE_4BIT_CLUT = 0,
	COLOR_MODE_8BIT_CLUT = 1,
	COLOR_MODE_15BIT_DIRECT = 2,
};

class draw_list
{
public:
	draw_list();

	void reset();
	void draw();

	void add_dot(int x, int y, const rgb& color);
	void add_rectangle(int x, int y, int width, int height, const rgb& colorb);
	void add_sprite_8x8(int x, int y, int u, int v, const rgb& color);
	void add_sprite_16x16(int x, int y, int u, int v, const rgb& color);
	void add_sprite(int x, int y, int u, int v, int width, int height, const rgb& color);
	void add_set_draw_mode(int texture_page, color_mode texture_color_mode);

private:
	template <typename... Values>
	void enqueue_data(uint32_t *data)
	{
		const int packet_size = (data - tail_) - 1;
		*tail_ = (packet_size << 24) | (reinterpret_cast<uint32_t>(data) & 0xffffff);
		tail_ = data;
	}

	template <typename... Values>
	void enqueue_data(uint32_t *data, uint32_t value, Values... values)
	{
		*data = value;
		enqueue_data(data + 1, values...);
	}

	template <typename... Values>
	void enqueue_packet(Values... values)
	{
		enqueue_data(tail_ + 1, values...);
	}

	void add_terminator();

	static const uint8_t PACKET_RECTANGLE		= 0x60;
	static const uint8_t PACKET_DOT			= 0x68;
	static const uint8_t PACKET_SPRITE		= 0x64;
	static const uint8_t PACKET_SPRITE_8		= 0x74;
	static const uint8_t PACKET_SPRITE_16		= 0x7c;
	static const uint8_t PACKET_SET_DRAW_MODE	= 0xe1;

	uint32_t data_[0x4000];
	uint32_t *tail_;
};

} };

#endif // DRAW_LIST_H_
