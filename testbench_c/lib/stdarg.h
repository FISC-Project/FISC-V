#ifndef STDARG_H_
#define STDARG_H_

#define FIXSTACK char __st1__,__st2__,__st3__,__st4__,__st5__,__st6__,__st7__,__st8__,__st9__,__st10__,__st11__,__st12__,__st13__,__st14__,__st15__,__st16__,__st17__,__st18__,__st19__,__st20__;

typedef __builtin_va_list   va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

#endif