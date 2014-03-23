#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <stdint.h>
#include <stddef.h>

class file_reader
{
public:
	file_reader(const char *path);
	virtual ~file_reader();

	void read(void *buf, size_t count);
	uint8_t read_uint8();
	uint16_t read_uint16();
	uint32_t read_uint32();
	char *read_string();

protected:
	void read_block();
	int fd_;
	static const size_t BLOCK_SIZE = 2048;
	char block_[BLOCK_SIZE];
	size_t block_index_;
};

#endif // FILE_READER_H_
