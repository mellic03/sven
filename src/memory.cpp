#include "memory.hpp"

#include <immintrin.h>



void
sven::memset32( void *ptr, uint32_t value, uint32_t count )
{
    uint32_t *dst = (uint32_t *)(ptr);

    __m128i  reg = _mm_set1_epi32(value);
    uint32_t rem = count % 4;

    for (uint32_t i=0; i<count; i+=4)
    {
        _mm_storeu_si128((__m128i *)(&dst[i]), reg);
    }

    for (uint32_t i=count-rem; i<count; i++)
    {
        dst[i] = value;
    }
}


void
sven::memcpy32( void *dst_ptr, const void *src_ptr, uint32_t count )
{
    __m128i *dst = (__m128i *)dst_ptr;
    const __m128i *src = (const __m128i *)src_ptr;

    __m128i reg;

    for (uint32_t i=0; i<count/4; i++)
    {
        reg = _mm_loadu_si128(src+i);
        _mm_storeu_si128(dst+i, reg);
    }
}
