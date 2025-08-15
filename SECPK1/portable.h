#ifndef PORTABLE_H
#define PORTABLE_H

#include <stdint.h>

// Portable implementation of _addcarry_u64
static inline uint8_t _addcarry_u64(uint8_t carry_in, uint64_t a, uint64_t b, uint64_t *result) {
    uint64_t sum = a + b;
    uint8_t carry1 = (sum < a);

    sum += carry_in;
    uint8_t carry2 = (sum < carry_in);

    *result = sum;
    return carry1 | carry2;
}

// Portable implementation of _subborrow_u64
static inline uint8_t _subborrow_u64(uint8_t borrow_in, uint64_t a, uint64_t b, uint64_t *result) {
    uint64_t diff = a - b;
    uint8_t borrow1 = (a < b);

    uint64_t diff2 = diff - borrow_in;
    uint8_t borrow2 = (diff < borrow_in);

    *result = diff2;
    return borrow1 | borrow2;
}

#endif // PORTABLE_H

static inline void mul_u64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo) {
#if defined(__x86_64__)
    __asm__("mulq %[b]"
            : "=d"(*rhi), "=a"(*rlo)
            : "1"(a), [b] "rm"(b));
#elif defined(__SIZEOF_INT128__)
    __uint128_t product = (__uint128_t)a * (__uint128_t)b;
    *rlo = (uint64_t)product;
    *rhi = (uint64_t)(product >> 64);
#else
    // Fallback 32-bit limb-based multiplication
    uint64_t a_lo = (uint32_t)a;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = (uint32_t)b;
    uint64_t b_hi = b >> 32;

    uint64_t lo_lo = a_lo * b_lo;
    uint64_t hi_lo = a_hi * b_lo;
    uint64_t lo_hi = a_lo * b_hi;
    uint64_t hi_hi = a_hi * b_hi;

    uint64_t mid1 = hi_lo + (lo_lo >> 32);
    uint64_t mid2 = lo_hi + (mid1 & 0xFFFFFFFF);

    *rlo = (mid1 << 32) | (lo_lo & 0xFFFFFFFF);
    *rhi = hi_hi + (mid1 >> 32) + (mid2 >> 32);
#endif
}

