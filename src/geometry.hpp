#pragma once

#include "vertex.hpp"
#include <vector>


namespace sven
{
    void tri_subdivide( const Vertex *src, std::vector<Vertex> &dst );

    std::vector<Vertex> gen_cube();
    std::vector<Vertex> gen_isosphere( uint32_t subdivisions );
}
