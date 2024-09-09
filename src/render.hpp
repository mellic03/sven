#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "vertex.hpp"


namespace sven
{
    void *vbuffer_new  ( uint32_t stride, uint32_t count );

    void drawArrays( const VertexBuffer &buf );
    void drawElements( const VertexBuffer &buf, const std::vector<uint32_t> &indices );
}

