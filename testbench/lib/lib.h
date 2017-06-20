#ifndef LIB_H_
#define LIB_H_

#define attr(attribute) __attribute__((attribute))

#define __packed attr(packed)
#define __used attr(used)
#define __unused attr(unused)

#define __section(s) attr(__section__(#s))
#define __cold attr(cold)

#define __visible attr(externally_visible)
#define __visibility(vis) attr(visibility(vis))
#define __vis_default "default"
#define __vis_hidden "hidden"
#define __vis_internal "internal"
#define __vis_protected "protected"

#define __text __section(.text)
#define __data __section(.data)

#define __deprecated(msg) attr(deprecated(msg))
#define __error(err) attr(error(msg))
#define __warning(warn) attr(warning(warn))

#define __interrupt attr(interrupt)

#define __pure attr(pure)
#define __weak attr(weak)

#define __align(al) attr(aligned(al))

#define __optimize attr(optimize)
#define __hot attr(hot)

#define __malloc attr(malloc)

#define __target(targ) attr(__target__(targ))

typedef unsigned char      uint8_t;
typedef signed   char      int8_t;
typedef unsigned short     uint16_t;
typedef signed   short     int16_t;
typedef unsigned int       uint32_t;
typedef signed   int       int32_t;
typedef unsigned long long uint64_t;
typedef signed   long long int64_t;

typedef unsigned int       size_t;

typedef uint32_t           uintptr_t;

/**********************************************/
/******************** HEAP ********************/
/**********************************************/
#define ALLOCLOC 0x400
uintptr_t alloc_head __data = ALLOCLOC;

void * malloc(size_t size)
{
	uintptr_t thisloc = alloc_head;
	alloc_head += size;
	return (void*)thisloc;
}

void memcpy(uint32_t * dst, uint32_t * src, int len)
{
	for(int i = 0; i <= len; i++)
		dst[i] = src[i];
}

void * memset(uint32_t * ptr, int val, int len)
{
	for(int i = 0; i <= len; i++)
		ptr[i] = val;
	return ptr;
}

/***********************************************/
/******************** DEBUG ********************/
/***********************************************/
#define DEBUGLOC 0x300
#define DEBUG(loc, intnum) (((int*)DEBUGLOC)[loc] = (int)intnum)

#endif
