#ifndef GPU_H_
#define GPU_H_

#include <stdint.h>

namespace psx { namespace gpu {

static uint32_t *GPU_CONTROL_PORT		= reinterpret_cast<uint32_t *>(0x1f801814);
static uint32_t *GPU_DATA_PORT			= reinterpret_cast<uint32_t *>(0x1f801810);
static uint32_t *DMA_BASE_ADDRESS_PORT		= reinterpret_cast<uint32_t *>(0x1f8010a0);
static uint32_t *DMA_BLOCK_CONTROL_PORT		= reinterpret_cast<uint32_t *>(0x1f8010a4);
static uint32_t *DMA_CHANNEL_CONTROL_PORT	= reinterpret_cast<uint32_t *>(0x1f8010a8);

enum dma_mode {
	DMA_DISABLED = 0,
	DMA_CPU_TO_GPU = 2,
	DMA_GPU_TO_CPU = 3
};

inline uint32_t
gpu_control_port()
{
	return *GPU_CONTROL_PORT;
}

inline void
gpu_control_port(uint32_t data)
{
	*GPU_CONTROL_PORT = data;
}

inline void
gpu_control_send_command(uint32_t command, uint32_t param)
{
	gpu_control_port((command << 0x18) | param);
}

inline void
gpu_control_reset_command_buffer()
{
	gpu_control_send_command(1, 0);
}

inline void
gpu_control_set_dma_mode(dma_mode mode)
{
	gpu_control_send_command(4, mode);
}

inline uint32_t
gpu_data_port()
{
	return *GPU_DATA_PORT;
}

inline void
gpu_data_port(uint32_t data)
{
	*GPU_DATA_PORT = data;
}

inline void
gpu_dma_set_base_address(uint32_t address)
{
	*DMA_BASE_ADDRESS_PORT = address;
}

inline void
gpu_dma_set_block_control(uint32_t value)
{
	*DMA_BLOCK_CONTROL_PORT = value;
}

inline void
gpu_dma_set_channel_control(uint32_t value)
{
	*DMA_CHANNEL_CONTROL_PORT = value;
}

} };

#endif // GPU_H_
