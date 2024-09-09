#pragma once

#include <cstdint>
#include <cstddef>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "vertex.hpp"
#include "context.hpp"


namespace sven
{
    static constexpr uint32_t COLOR_BUFFER_IDX = 0;
    static constexpr uint32_t DEPTH_BUFFER_IDX = 1;

    static constexpr uint32_t COLOR_BUFFER_BIT = 1 << COLOR_BUFFER_IDX;
    static constexpr uint32_t DEPTH_BUFFER_BIT = 1 << DEPTH_BUFFER_IDX;

}


namespace sven::internal
{
    void rasterize( const VaryingArray &buf, int idx,
                    Texture &dst_depth, Texture &dst_color, Texture &src_color,
                    const RenderState &RS );
}

