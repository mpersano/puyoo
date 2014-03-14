#include "gpu.h"
#include "draw_list.h"

namespace psx { namespace gpu {

draw_list::draw_list()
{
	reset();
}

void
draw_list::reset()
{
	tail_ = data_;
}

void
draw_list::add_dot(int x, int y, const rgb& color)
{
	enqueue_packet(
		(PACKET_DOT << 24) | color,
		(y << 16) | x);
}

void
draw_list::add_rectangle(int x, int y, int width, int height, const rgb& color)
{
	enqueue_packet(
		(PACKET_RECTANGLE << 24) | color,
		(y << 16) | x,
		(height << 16) | width);
}

void
draw_list::add_sprite_8x8(int x, int y, int u, int v, const rgb& color)
{
	enqueue_packet(
		(PACKET_SPRITE_8 << 24) | color,
		(y << 16) | x,
		(v << 8) | u);
}

void
draw_list::add_sprite_16x16(int x, int y, int u, int v, const rgb& color)
{
	enqueue_packet(
		(PACKET_SPRITE_16 << 24) | color,
		(y << 16) | x,
		(v << 8) | u);
}

void
draw_list::add_sprite(int x, int y, int u, int v, int width, int height, const rgb& color)
{
	enqueue_packet(
		(PACKET_SPRITE << 24) | color,
		(y << 16) | x,
		(v << 8) | u,
		(height << 16) | width);
}

void
draw_list::add_set_draw_mode(int texture_page, color_mode texture_color_mode)
{
	enqueue_packet(
		(PACKET_SET_DRAW_MODE << 24) | (texture_color_mode << 7) | texture_page);
}

void
draw_list::add_terminator()
{
	*tail_ = 0x00ffffff;
}

void
draw_list::draw()
{
	add_terminator();

        while(!(gpu_control_port() & (1 << 0x1a)))
		;

        while(!(gpu_control_port() & (1 << 0x1c)))
		;

	gpu_control_set_dma_mode(DMA_CPU_TO_GPU);
	gpu_dma_set_base_address(reinterpret_cast<uint32_t>(data_));
	gpu_dma_set_block_control(1);
	gpu_dma_set_channel_control((1 << 0xa) | 1 | (1 << 0x18));
}

} }
