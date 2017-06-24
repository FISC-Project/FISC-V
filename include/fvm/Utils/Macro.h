#ifndef UTILS_MACRO_H_
#define UTILS_MACRO_H_

#define EXPAND(x) x

// FOREACH macros:
#define FE_1(WHAT,  X)      WHAT(X)
#define FE_2(WHAT,  X, ...) WHAT(X)EXPAND(FE_1(WHAT,  __VA_ARGS__))
#define FE_3(WHAT,  X, ...) WHAT(X)EXPAND(FE_2(WHAT,  __VA_ARGS__))
#define FE_4(WHAT,  X, ...) WHAT(X)EXPAND(FE_3(WHAT,  __VA_ARGS__))
#define FE_5(WHAT,  X, ...) WHAT(X)EXPAND(FE_4(WHAT,  __VA_ARGS__))
#define FE_6(WHAT,  X, ...) WHAT(X)EXPAND(FE_5(WHAT,  __VA_ARGS__))
#define FE_7(WHAT,  X, ...) WHAT(X)EXPAND(FE_6(WHAT,  __VA_ARGS__))
#define FE_8(WHAT,  X, ...) WHAT(X)EXPAND(FE_7(WHAT,  __VA_ARGS__))
#define FE_9(WHAT,  X, ...) WHAT(X)EXPAND(FE_8(WHAT,  __VA_ARGS__))
#define FE_10(WHAT, X, ...) WHAT(X)EXPAND(FE_9(WHAT,  __VA_ARGS__))
#define FE_11(WHAT, X, ...) WHAT(X)EXPAND(FE_10(WHAT, __VA_ARGS__))
#define FE_12(WHAT, X, ...) WHAT(X)EXPAND(FE_11(WHAT, __VA_ARGS__))
#define FE_13(WHAT, X, ...) WHAT(X)EXPAND(FE_12(WHAT, __VA_ARGS__))
#define FE_14(WHAT, X, ...) WHAT(X)EXPAND(FE_13(WHAT, __VA_ARGS__))
#define FE_15(WHAT, X, ...) WHAT(X)EXPAND(FE_14(WHAT, __VA_ARGS__))
#define FE_16(WHAT, X, ...) WHAT(X)EXPAND(FE_15(WHAT, __VA_ARGS__))
#define FE_17(WHAT, X, ...) WHAT(X)EXPAND(FE_16(WHAT, __VA_ARGS__))
#define FE_18(WHAT, X, ...) WHAT(X)EXPAND(FE_17(WHAT, __VA_ARGS__))
#define FE_19(WHAT, X, ...) WHAT(X)EXPAND(FE_18(WHAT, __VA_ARGS__))
#define FE_20(WHAT, X, ...) WHAT(X)EXPAND(FE_19(WHAT, __VA_ARGS__))

#define FE2_1(WHAT,  X, A)                                                          WHAT(X,A)
#define FE2_2(WHAT,  X, A, B)                                                       WHAT(X,A)FE2_1(WHAT,  X, B)
#define FE2_3(WHAT,  X, A, B, C)                                                    WHAT(X,A)FE2_2(WHAT,  X, B, C)
#define FE2_4(WHAT,  X, A, B, C, D)                                                 WHAT(X,A)FE2_3(WHAT,  X, B, C, D)
#define FE2_5(WHAT,  X, A, B, C, D, E)                                              WHAT(X,A)FE2_4(WHAT,  X, B, C, D, E)
#define FE2_6(WHAT,  X, A, B, C, D, E, F)                                           WHAT(X,A)FE2_5(WHAT,  X, B, C, D, E, F)
#define FE2_7(WHAT,  X, A, B, C, D, E, F, G)                                        WHAT(X,A)FE2_6(WHAT,  X, B, C, D, E, F, G)
#define FE2_8(WHAT,  X, A, B, C, D, E, F, G, H)                                     WHAT(X,A)FE2_7(WHAT,  X, B, C, D, E, F, G, H)
#define FE2_9(WHAT,  X, A, B, C, D, E, F, G, H, I)                                  WHAT(X,A)FE2_8(WHAT,  X, B, C, D, E, F, G, H, I)
#define FE2_10(WHAT, X, A, B, C, D, E, F, G, H, I, J)                               WHAT(X,A)FE2_9(WHAT,  X, B, C, D, E, F, G, H, I, J)
#define FE2_11(WHAT, X, A, B, C, D, E, F, G, H, I, J, K)                            WHAT(X,A)FE2_10(WHAT, X, B, C, D, E, F, G, H, I, J, K)
#define FE2_12(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L)                         WHAT(X,A)FE2_11(WHAT, X, B, C, D, E, F, G, H, I, J, K, L)
#define FE2_13(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M)                      WHAT(X,A)FE2_12(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M)
#define FE2_14(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N)                   WHAT(X,A)FE2_13(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M. N)
#define FE2_15(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O)                WHAT(X,A)FE2_14(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O)
#define FE2_16(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)             WHAT(X,A)FE2_15(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)
#define FE2_17(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q)          WHAT(X,A)FE2_16(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q)
#define FE2_18(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R)       WHAT(X,A)FE2_17(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R)
#define FE2_19(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S)    WHAT(X,A)FE2_18(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S)
#define FE2_20(WHAT, X, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T) WHAT(X,A)FE2_19(WHAT, X, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,NAME,...) NAME
#define FOR_EACH(action,...) EXPAND(GET_MACRO(__VA_ARGS__,FE_20,FE_19,FE_18,FE_17,FE_16,FE_15,FE_14,FE_13,FE_12,FE_11,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1)(action,__VA_ARGS__))
#define FOR_EACH_ARG(action,foreacharg,...) EXPAND(GET_MACRO(__VA_ARGS__,FE2_20,FE2_19,FE2_18,FE2_17,FE2_16,FE2_15,FE2_14,FE2_13,FE2_12,FE2_11,FE2_10,FE2_9,FE2_8,FE2_7,FE2_6,FE2_5,FE2_4,FE2_3,FE2_2,FE2_1)(action, foreacharg, __VA_ARGS__))

#endif