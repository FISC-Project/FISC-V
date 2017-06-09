#ifndef UTILS_MACRO_H_
#define UTILS_MACRO_H_

#define EXPAND(x) x

// FOREACH macros:
#define FE_1(WHAT, X) WHAT(X) 
#define FE_2(WHAT, X, ...) WHAT(X)EXPAND(FE_1(WHAT, __VA_ARGS__))
#define FE_3(WHAT, X, ...) WHAT(X)EXPAND(FE_2(WHAT, __VA_ARGS__))
#define FE_4(WHAT, X, ...) WHAT(X)EXPAND(FE_3(WHAT, __VA_ARGS__))
#define FE_5(WHAT, X, ...) WHAT(X)EXPAND(FE_4(WHAT, __VA_ARGS__))

#define FE2_1(WHAT, X, A) WHAT(X,A)
#define FE2_2(WHAT, X, A, B) WHAT(X,A)FE2_1(WHAT, X, B)
#define FE2_3(WHAT, X, A, B, C) WHAT(X,A)FE2_2(WHAT, X, B, C)
#define FE2_4(WHAT, X, A, B, C, D) WHAT(X,A)FE2_3(WHAT, X, B, C, D)
#define FE2_5(WHAT, X, A, B, C, D, E) WHAT(X,A)FE2_4(WHAT, X, B, C, D, E)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define FOR_EACH(action,...) EXPAND(GET_MACRO(__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1)(action,__VA_ARGS__))
#define FOR_EACH_ARG(action,foreacharg,...) EXPAND(GET_MACRO(__VA_ARGS__,FE2_5,FE2_4,FE2_3,FE2_2,FE2_1)(action, foreacharg, __VA_ARGS__))

#endif