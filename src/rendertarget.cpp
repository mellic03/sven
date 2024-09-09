#include "rendertarget.hpp"
#include <array>


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



void
sven::vertex_binner::clear()
{

}


std::array<int, 4> extract_bounds( const sven::Primitive &p, uint32_t grid_w, uint32_t grid_h )
{
    std::array<int, 4> bounds = { +INFINITY, -INFINITY, +INFINITY, -INFINITY };

    bounds[0] = std::min(p.pos[0].x, std::min(p.pos[1].x, p.pos[2].x)) / grid_w;
    bounds[1] = std::max(p.pos[0].x, std::max(p.pos[1].x, p.pos[2].x)) / grid_w;
    bounds[2] = std::min(p.pos[0].y, std::min(p.pos[1].y, p.pos[2].y)) / grid_h;
    bounds[3] = std::max(p.pos[0].y, std::max(p.pos[1].y, p.pos[2].y)) / grid_h;

    return bounds;
}



/**
 * @param buf buffer of screen space vertices before rasterization.
 */
int
sven::vertex_binner::insert( const std::vector<Primitive> &buf )
{
    uint32_t grid_w = w/TILE_W;
    uint32_t grid_h = h/TILE_W;

    for (const Primitive &P: buf)
    {
        auto [xmin, xmax, ymin, ymax] = extract_bounds(P, grid_w, grid_h);

        for (int y=ymin; y<ymax; y++)
        {
            for (int x=xmin; x<xmax; x++)
            {
                vertex_bins[grid_w*y + x].push_back(P);
            }
        }
    }


    return 0;
}




