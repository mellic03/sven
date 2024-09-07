#pragma once

#include <cstdint>
#include <cstddef>


namespace sven
{
    void memset32( uint32_t *dst, uint32_t value, uint32_t count );
    void memcpy32( void *dst, const void *src, uint32_t count );
}

