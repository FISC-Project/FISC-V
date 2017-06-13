#ifndef BIT_H_
#define BIT_H_

#define ALIGN16(addr)   ((addr)<<1)
#define ALIGN32(addr)   ((addr)<<2)
#define ALIGN64(addr)   ((addr)<<3)
#define ALIGN128(addr)  ((addr)<<4)
#define ALIGN256(addr)  ((addr)<<5)
#define ALIGN512(addr)  ((addr)<<6)
#define ALIGN1024(addr) ((addr)<<7)
#define ALIGN2048(addr) ((addr)<<8)
#define ALIGN4096(addr) ((addr)<<9)
#define ALIGN8192(addr) ((addr)<<10)

#define UNALIGN16(addr)   ((addr)>>1)
#define UNALIGN32(addr)   ((addr)>>2)
#define UNALIGN64(addr)   ((addr)>>3)
#define UNALIGN128(addr)  ((addr)>>4)
#define UNALIGN256(addr)  ((addr)>>5)
#define UNALIGN512(addr)  ((addr)>>6)
#define UNALIGN1024(addr) ((addr)>>7)
#define UNALIGN2048(addr) ((addr)>>8)
#define UNALIGN4096(addr) ((addr)>>9)
#define UNALIGN8192(addr) ((addr)>>10)

#endif