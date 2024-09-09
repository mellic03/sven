#pragma once

#include <vector>
#include "vertex.hpp"
#include "texture.hpp"


namespace sven
{
    struct vertex_binner;
}


struct sven::vertex_binner
{
    static constexpr uint32_t TILE_W = 32;
    static constexpr uint32_t MAX_VBUFFER_SIZE = 512;

    uint32_t w, h;
    std::vector<uint32_t *>    color_tiles;
    std::vector<float *>       depth_tiles;
    std::vector<std::vector<Primitive>>  vertex_bins;

    vertex_binner( uint32_t width, uint32_t height );

    void clear();
    int  insert( const std::vector<Primitive>& );

};


