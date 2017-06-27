#ifndef STDLIB_H_
#define STDLIB_H_

#define ALLOCLOC 0x20000
uintptr_t alloc_head __data = ALLOCLOC;

void * malloc(size_t size)
{
	uintptr_t thisloc = alloc_head;
	alloc_head += size;
	return (void*)thisloc;
}

void * memcpy(void * dst, void * src, size_t len)
{
	uint8_t * dst_ = (uint8_t*)dst;
	uint8_t * src_ = (uint8_t*)src;
	for(size_t i = 0; i < len; i++)
		dst_[i] = src_[i];
	return dst;
}

void * memset(void * dst, int32_t val, size_t len)
{
	uint8_t * dst_ = (uint8_t*)dst;
	for(size_t i = 0; i < len; i++)
		dst_[i] = val;
	return dst;
}

#endif