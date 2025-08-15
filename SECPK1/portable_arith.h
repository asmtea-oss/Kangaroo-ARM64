#ifndef PORTABLE_ARITH_H
#define PORTABLE_ARITH_H

#include <stdint.h>

//
// Portable 64x64 -> 128 Unsigned Multiply
//
static inline uint64_t _umul128(uint64_t a, uint64_t b, uint64_t *h) {
#if defined(__x86_64__)
    uint64_t rhi, rlo;
    __asm__("mulq %[b]" : "=d"(rhi), "=a"(rlo) : "1"(a), [b]"rm"(b));
    *h = rhi;
    return rlo;
#elif defined(__SIZEOF_INT128__)
    __uint128_t result = (__uint128_t)a * (__uint128_t)b;
    *h = (uint64_t)(result >> 64);
    return (uint64_t)result;
#else
    // Portable 32-bit limb fallback
    uint64_t a_lo = (uint32_t)a;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = (uint32_t)b;
    uint64_t b_hi = b >> 32;

    uint64_t lo_lo = a_lo * b_lo;
    uint64_t hi_lo = a_hi * b_lo;
    uint64_t lo_hi = a_lo * b_hi;
    uint64_t hi_hi = a_hi * b_hi;

    uint64_t mid = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFF) + (lo_hi & 0xFFFFFFFF);
    *h = hi_hi + (hi_lo >> 32) + (lo_hi >> 32) + (mid >> 32);
    return (mid << 32) | (lo_lo & 0xFFFFFFFF);
#endif
}

//
// Portable 64x64 -> 128 Signed Multiply
//
static inline int64_t _mul128(int64_t a, int64_t b, int64_t *h) {
#if defined(__x86_64__)
    int64_t rhi, rlo;
    __asm__("imulq %[b]" : "=d"(rhi), "=a"(rlo) : "1"(a), [b]"rm"(b));
    *h = rhi;
    return rlo;
#elif defined(__SIZEOF_INT128__)
    __int128 result = (__int128)a * (__int128)b;
    *h = (int64_t)(result >> 64);
    return (int64_t)result;
#else
    #error "_mul128 not supported on this platform without __int128"
#endif
}

//
// Portable Unsigned 128 ÷ 64 Division
//
static inline uint64_t _udiv128(uint64_t hi, uint64_t lo, uint64_t d, uint64_t *r) {
#if defined(__x86_64__)
    uint64_t q, rem;
    __asm__("divq %[d]" : "=a"(q), "=d"(rem) : "a"(lo), "d"(hi), [d]"rm"(d));
    *r = rem;
    return q;
#elif defined(__SIZEOF_INT128__)
    __uint128_t dividend = ((__uint128_t)hi << 64) | lo;
    *r = (uint64_t)(dividend % d);
    return (uint64_t)(dividend / d);
#else
    #error "_udiv128 not supported on this platform without __int128"
#endif
}

//
// Portable Timestamp Counter (x86/ARM)
//
static inline uint64_t __rdtsc() {
#if defined(__x86_64__)
    uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
#elif defined(__aarch64__)
    uint64_t cntvct;
    asm volatile("mrs %0, cntvct_el0" : "=r"(cntvct));
    return cntvct;
#else
    #error "__rdtsc not supported on this platform"
#endif
}

#endif // PORTABLE_ARITH_H

