#pragma once

#include "vertex.hpp"
#include <vector>


namespace sven
{
    void tri_subdivide( const VertexArray &src, int idx, VertexArray &dst );

    void gen_quad( sven::VertexArray &dst, int idx );
    sven::VertexArray gen_quad( float xscale, float zscale, float uscale, float vscale, int subdivisions=1 );
    sven::VertexArray gen_cube();
    sven::VertexArray gen_isosphere( uint32_t subdivisions );
}
