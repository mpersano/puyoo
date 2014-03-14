#include <types.h>

extern "C" {
void *malloc(size_t);
void free(void *);
};

void *
operator new(size_t size)
{
	return malloc(size);
}

void *
operator new[](size_t size)
{
	return malloc(size);
}

void
operator delete(void *ptr)
{
	free(ptr);
}

void
operator delete[](void *ptr)
{
	free(ptr);
}

extern "C" int
atexit(void (*)())
{
	return 0;
}
