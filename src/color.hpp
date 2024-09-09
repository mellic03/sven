#pragma once

#include <cstdint>
#include <cstddef>
#include <glm/glm.hpp>

// #include "texture.hpp"
#include "vertex.hpp"

namespace sven
{
    uint32_t packR( uint32_t r );
    uint32_t packRGB( uint32_t r, uint32_t g, uint32_t b );
    uint32_t packRGBA( uint32_t r, uint32_t g, uint32_t b, uint32_t a );

    uint32_t packRGBf( const glm::vec3& );
    uint32_t packRGBAf( const glm::vec4& );
    glm::vec4 unpackRGBAf( uint32_t fill );
}