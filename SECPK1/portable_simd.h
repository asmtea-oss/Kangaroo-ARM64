#include <stdint.h>

#if defined(__x86_64__)
  #include <emmintrin.h>  // SSE2
#elif defined(__aarch64__)
  #include <arm_neon.h>
#endif

// 128-bit vector of two uint64_t values
typedef union {
    uint64_t u64[2];
#if defined(__x86_64__)
    __m128i   m;
#elif defined(__aarch64__)
    uint64x2_t n;
#endif
} vec128_u64;

// Portable shift left 64-bit lanes
static inline vec128_u64 vec_slli_epi64(vec128_u64 v, int shift) {
    vec128_u64 result;
#if defined(__x86_64__)
    result.m = _mm_slli_epi64(v.m, shift);
#elif defined(__aarch64__)
    int64x2_t shift_vec = vdupq_n_s64((int64_t)shift);  // signed vector!
    result.n = vshlq_u64(v.n, shift_vec);               // shift unsigned vector by signed amount
#else
    result.u64[0] = v.u64[0] << shift;
    result.u64[1] = v.u64[1] << shift;
#endif
    return result;
}


// Portable subtraction of two 128-bit vectors (per-lane)
static inline vec128_u64 vec_sub_epi64(vec128_u64 a, vec128_u64 b) {
    vec128_u64 result;
#if defined(__x86_64__)
    result.m = _mm_sub_epi64(a.m, b.m);
#elif defined(__aarch64__)
    result.n = vsubq_u64(a.n, b.n);
#else
    result.u64[0] = a.u64[0] - b.u64[0];
    result.u64[1] = a.u64[1] - b.u64[1];
#endif
    return result;
}

