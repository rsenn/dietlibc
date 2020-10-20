/* origin: FreeBSD /usr/src/lib/msun/src/math_private.h */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#ifndef _LIBM_H
#define _LIBM_H

#include <stdint.h>
#include <float.h>
#include "math.h"
#include "complex.h"
#include <endian.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
  long double f;
  struct {
    uint64_t m;
    uint16_t se;
  } i;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
  long double f;
  struct {
    uint64_t lo;
    uint32_t mid;
    uint16_t top;
    uint16_t se;
  } i;
  struct {
    uint64_t lo;
    uint64_t hi;
  } i2;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __BIG_ENDIAN
union ldshape {
  long double f;
  struct {
    uint16_t se;
    uint16_t top;
    uint32_t mid;
    uint64_t lo;
  } i;
  struct {
    uint64_t hi;
    uint64_t lo;
  } i2;
};
#else
#error Unsupported long double representation
#endif

#define FORCE_EVAL(x)                                                                                                  \
  do {                                                                                                                 \
    if(sizeof(x) == sizeof(float)) {                                                                                   \
      volatile float __x;                                                                                              \
      __x = (x);                                                                                                       \
    } else if(sizeof(x) == sizeof(double)) {                                                                           \
      volatile double __x;                                                                                             \
      __x = (x);                                                                                                       \
    } else {                                                                                                           \
      volatile long double __x;                                                                                        \
      __x = (x);                                                                                                       \
    }                                                                                                                  \
  } while(0)

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(hi, lo, d)                                                                                       \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    (hi) = __u.i >> 32;                                                                                                \
    (lo) = (uint32_t)__u.i;                                                                                            \
  } while(0)

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(hi, d)                                                                                           \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    (hi) = __u.i >> 32;                                                                                                \
  } while(0)

/* Get the less significant 32 bit int from a double.  */
#define GET_LOW_WORD(lo, d)                                                                                            \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    (lo) = (uint32_t)__u.i;                                                                                            \
  } while(0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d, hi, lo)                                                                                        \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.i = ((uint64_t)(hi) << 32) | (uint32_t)(lo);                                                                   \
    (d) = __u.f;                                                                                                       \
  } while(0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d, hi)                                                                                           \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    __u.i &= 0xffffffff;                                                                                               \
    __u.i |= (uint64_t)(hi) << 32;                                                                                     \
    (d) = __u.f;                                                                                                       \
  } while(0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d, lo)                                                                                            \
  do {                                                                                                                 \
    union {                                                                                                            \
      double f;                                                                                                        \
      uint64_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    __u.i &= 0xffffffff00000000ull;                                                                                    \
    __u.i |= (uint32_t)(lo);                                                                                           \
    (d) = __u.f;                                                                                                       \
  } while(0)

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(w, d)                                                                                           \
  do {                                                                                                                 \
    union {                                                                                                            \
      float f;                                                                                                         \
      uint32_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.f = (d);                                                                                                       \
    (w) = __u.i;                                                                                                       \
  } while(0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d, w)                                                                                           \
  do {                                                                                                                 \
    union {                                                                                                            \
      float f;                                                                                                         \
      uint32_t i;                                                                                                      \
    } __u;                                                                                                             \
    __u.i = (w);                                                                                                       \
    (d) = __u.f;                                                                                                       \
  } while(0)

#undef __CMPLX
#undef CMPLX
#undef CMPLXF
#undef CMPLXL

#define __CMPLX(x, y, t)                                                                                               \
  ((union {                                                                                                            \
     _Complex t __z;                                                                                                   \
     t __xy[2];                                                                                                        \
   }){.__xy = {(x), (y)}}                                                                                              \
       .__z)

#define CMPLX(x, y) __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

#define WANT_ROUNDING 1
#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

/* fdlibm kernel functions */

int __rem_pio2_large(double*, double*, int, int, int);

int __rem_pio2(double, double*);
double __sin(double, double, int);
double __cos(double, double);
double __tan(double, double, int);
double __expo2(double, double);
double complex __ldexp_cexp(double complex, int);

int __rem_pio2f(float, double*);
float __sindf(double);
float __cosdf(double);
float __tandf(double, int);
float __expo2f(float);
float complex __ldexp_cexpf(float complex, int);

int __rem_pio2l(long double, long double*);
long double __sinl(long double, long double, int);
long double __cosl(long double, long double);
long double __tanl(long double, long double, int);

/* polynomial evaluation */
long double __polevll(long double, const long double*, int);
long double __p1evll(long double, const long double*, int);

/* fp_barrier returns its input, but limits code transformations
   as if it had a side-effect (e.g. observable io) and returned
   an arbitrary value.  */

#ifndef fp_barrierf
#define fp_barrierf fp_barrierf
static inline float fp_barrierf(float x)
{
        volatile float y = x;
        return y;
}
#endif

#ifndef fp_barrier
#define fp_barrier fp_barrier
static inline double fp_barrier(double x)
{
        volatile double y = x;
        return y;
}
#endif

#ifndef fp_barrierl
#define fp_barrierl fp_barrierl
static inline long double fp_barrierl(long double x)
{
        volatile long double y = x;
        return y;
}
#endif

double __math_divzero(uint32_t sign);
double __math_invalid(double x);
double __math_oflow(uint32_t sign);
double __math_uflow(uint32_t sign);
double __math_xflow(uint32_t sign, double y);

/* Evaluate an expression as the specified type. With standard excess
   precision handling a type cast or assignment is enough (with
   -ffloat-store an assignment is required, in old compilers argument
   passing and return statement may not drop excess precision).  */

static inline float eval_as_float(float x)
{
  float y = x;
  return y;
}

static inline double eval_as_double(double x)
{
  double y = x;
  return y;
}

/* Helps static branch prediction so hot path can be better optimized.  */
#ifdef __GNUC__
#define predict_true(x) __builtin_expect(!!(x), 1)
#define predict_false(x) __builtin_expect(x, 0)
#else
#define predict_true(x) (x)
#define predict_false(x) (x)
#endif

#define asuint(f) ((union{float _f; uint32_t _i;}){f})._i
#define asfloat(i) ((union{uint32_t _i; float _f;}){i})._f
#define asuint64(f) ((union{double _f; uint64_t _i;}){f})._i
#define asdouble(i) ((union{uint64_t _i; double _f;}){i})._f

#endif
