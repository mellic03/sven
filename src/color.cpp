#include "color.hpp"


uint32_t
sven::packR( uint32_t r )
{
    return packRGBA(r, r, r, r);
}

uint32_t
sven::packRGB( uint32_t r, uint32_t g, uint32_t b )
{
    return packRGBA(r, g, b, 255);
}

uint32_t
sven::packRGBA( uint32_t r, uint32_t g, uint32_t b, uint32_t a )
{
    return (a << 24) + (r << 16) + (g << 8) + (b << 0);
}



uint32_t
sven::packRGBf( const glm::vec3 &color )
{
    return packRGBAf(glm::vec4(color, 1.0f));
}

uint32_t
sven::packRGBAf( const glm::vec4 &color )
{
    uint32_t a = uint32_t(255.0f * color.a) << 24;
    uint32_t r = uint32_t(255.0f * color.r) << 16;
    uint32_t g = uint32_t(255.0f * color.g) << 8;
    uint32_t b = uint32_t(255.0f * color.b) << 0;

    return a + r + g + b;
}


glm::vec4
sven::unpackRGBAf( uint32_t fill )
{
    return glm::vec4(uint8_t(fill>>16), uint8_t(fill>>8), uint8_t(fill>>0), uint8_t(fill>>24)) / 255.0f;
}
