#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include "panic.h"
#include "file_reader.h"

template <typename T>
static inline T min(T a, T b)
{ return a < b ? a : b; }

file_reader::file_reader(const char *path)
: fd_(::open(path, O_RDONLY))
, block_index_(0)
{
	if (fd_ == -1)
		panic("failed to open `%s'", path);

	read_block();
}

file_reader::~file_reader()
{
	::close(fd_);
}

uint8_t
file_reader::read_uint8()
{
	if (block_index_ == BLOCK_SIZE) {
		read_block();
		block_index_ = 0;
	}

	return block_[block_index_++];
}

uint16_t
file_reader::read_uint16()
{
	uint16_t lo = static_cast<uint16_t>(read_uint8());
	uint16_t hi = static_cast<uint16_t>(read_uint8());
	return lo | (hi << 8);
}

uint32_t
file_reader::read_uint32()
{
	uint32_t lo = static_cast<uint32_t>(read_uint16());
	uint32_t hi = static_cast<uint32_t>(read_uint16());
	return lo | (hi << 16);
}

void
file_reader::read(void *buf, size_t count)
{
	while (count) {
		if (block_index_ == BLOCK_SIZE) {
			read_block();
			block_index_ = 0;
		}

		size_t to_copy = min(count, BLOCK_SIZE - block_index_);
		memcpy(buf, &block_[block_index_], to_copy);

		block_index_ += to_copy;
		buf += to_copy;
		count -= to_copy;
	}
}

char *
file_reader::read_string()
{
	uint8_t len = read_uint8();

	char *str = new char[len + 1];
	read(str, len);
	str[len] = '\0';

	return str;
}

void
file_reader::read_block()
{
	::read(fd_, block_, BLOCK_SIZE);
}
