#ifndef ATTR_H_
#define ATTR_H_

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

#endif