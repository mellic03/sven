#include "rendertarget.hpp"


sven::vertex_binner::vertex_binner( uint32_t width, uint32_t height )
:   w(width),
    h(height),
    color_tiles(w*h),
    depth_tiles(w*h),
    vertex_bins(w*h, VertexBuffer(MAX_VBUFFER_SIZE))
{
    if (w == 0 || h == 0 || w%TILE_W != 0 || h%TILE_W != 0)
    {
        std::cout << "w, h must be divisble by TILE_W!\n";
        exit(1);
    }

    uint32_t grid_w = w/TILE_W;
    uint32_t grid_h = h/TILE_W;

    for (uint32_t i=0; i<grid_w*grid_h; i++)
    {
        color_tiles[i] = new uint32_t[TILE_W*TILE_W];
        depth_tiles[i] = new float[TILE_W*TILE_W];

        sven::memset32(color_tiles[i], 0, TILE_W*TILE_W);
        sven::memset32(depth_tiles[i], 0, TILE_W*TILE_W);
    }
}