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

typedef char *             string;

#define nullptr ((void*)0)

/**********************************************/
/******************** HEAP ********************/
/**********************************************/
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

/***********************************************/
/******************** DEBUG ********************/
/***********************************************/
#define DEBUGLOC 0x10000
#define DEBUG(loc, intnum) (((uint32_t*)DEBUGLOC)[(loc)] = ((uint32_t)(intnum)))
#define DEBUGLIST(listsize, list) for(size_t _i_ = 0; _i_ < ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_]));
#define DEBUGLISTOFF(offset, listsize, list) for(size_t _i_ = ((size_t)(offset)); _i_ < ((size_t)(offset)) + ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_ - ((size_t)(offset))]));

/************************************************/
/******************** STRING ********************/
/************************************************/
size_t strlen(string str)
{
	size_t size = 0;
	while(str[size] != '\0')
		size++;
	return size;
}

/************************************************/
/******************* IOSTREAM *******************/
/************************************************/
#define IOSPACE_VMCONSOLE_OUT 0x5000
#define IOSPACE_VMCONSOLE_IN  0x5001

void putc(char ch)
{
	*((char*)IOSPACE_VMCONSOLE_OUT) = (char)ch;
}

#define puts(str) do { static char str_[] __data = str; for(int i = 0; i < strlen(str_); i++) putc(str_[i]); } while(0)

#endif
