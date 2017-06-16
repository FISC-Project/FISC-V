#ifndef BIT_H_
#define BIT_H_

#define NTH_BIT(nth) (1 << (nth))
#define IS_BIT_SET(flag, nth) (flag) & NTH_BIT((nth))
#define FETCH_BIT(val, nth) (IS_BIT_SET((val), (nth)) >> (nth)))

#define BIT_SET(p, n) ((p) |= NTH_BIT((n)))
#define BIT_CLEAR(p, n) ((p) &= ~NTH_BIT((n)))
#define BIT_WRITE(c, p, n) (c ? BIT_SET(p, n) : BIT_CLEAR(p, n))

#define INDEX_FROM_BIT(b,  arr_size) ((b) / (arr_size))
#define OFFSET_FROM_BIT(b, arr_size) ((b) % (arr_size))

#define INDEX_FROM_BIT_SZ8(b)   INDEX_FROM_BIT(b,   8)
#define OFFSET_FROM_BIT_SZ8(b)  OFFSET_FROM_BIT(b,  8)
#define INDEX_FROM_BIT_SZ16(b)  INDEX_FROM_BIT(b,  16)
#define OFFSET_FROM_BIT_SZ16(b) OFFSET_FROM_BIT(b, 16)
#define INDEX_FROM_BIT_SZ32(b)  INDEX_FROM_BIT(b,  32)
#define OFFSET_FROM_BIT_SZ32(b) OFFSET_FROM_BIT(b, 32)
#define INDEX_FROM_BIT_SZ64(b)  INDEX_FROM_BIT(b,  64)
#define OFFSET_FROM_BIT_SZ64(b) OFFSET_FROM_BIT(b, 64)

#define ALIGN16(addr)     ((addr)<<1 )
#define ALIGN32(addr)     ((addr)<<2 )
#define ALIGN64(addr)     ((addr)<<3 )
#define ALIGN128(addr)    ((addr)<<4 )
#define ALIGN256(addr)    ((addr)<<5 )
#define ALIGN512(addr)    ((addr)<<6 )
#define ALIGN1024(addr)   ((addr)<<7 )
#define ALIGN2048(addr)   ((addr)<<8 )
#define ALIGN4096(addr)   ((addr)<<9 )
#define ALIGN8192(addr)   ((addr)<<10)

#define UNALIGN16(addr)   ((addr)>>1 )
#define UNALIGN32(addr)   ((addr)>>2 )
#define UNALIGN64(addr)   ((addr)>>3 )
#define UNALIGN128(addr)  ((addr)>>4 )
#define UNALIGN256(addr)  ((addr)>>5 )
#define UNALIGN512(addr)  ((addr)>>6 )
#define UNALIGN1024(addr) ((addr)>>7 )
#define UNALIGN2048(addr) ((addr)>>8 )
#define UNALIGN4096(addr) ((addr)>>9 )
#define UNALIGN8192(addr) ((addr)>>10)

#endif