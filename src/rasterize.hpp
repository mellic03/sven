#pragma once

#include <cstdint>
#include <cstddef>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "vertex.hpp"


namespace sven
{
    static constexpr uint32_t COLOR_BUFFER_IDX = 0;
    static constexpr uint32_t DEPTH_BUFFER_IDX = 1;

    static constexpr uint32_t COLOR_BUFFER_BIT = 1 << COLOR_BUFFER_IDX;
    static constexpr uint32_t DEPTH_BUFFER_BIT = 1 << DEPTH_BUFFER_IDX;


    Texture *init( uint32_t w, uint32_t h );
    glm::vec2 getRenderTargetSize();

    // void quad( float x, float y, float w, float h, float theta, uint32_t fill );

    uint32_t packR( uint32_t r );
    uint32_t packRGB( uint32_t r, uint32_t g, uint32_t b );
    uint32_t packRGBA( uint32_t r, uint32_t g, uint32_t b, uint32_t a );

}


namespace sven::internal
{
    void rasterize( const Vertex *buf, Texture &dst_depth, Texture &dst_color );
}

