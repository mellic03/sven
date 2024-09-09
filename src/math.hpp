#pragma once

#include <cmath>
#include <glm/glm.hpp>

namespace sven
{
    glm::vec4 vec4_rotate_xy( const glm::vec4&, float theta );
    void vec3_mat4_mul( float *v, float *M );


    /** Barycentric interpolation.
     * 
     */
    template <typename T>
    T baryp( const T &a, const T &b, const T &c, float *weights )
    {
        return a*weights[0] + b*weights[1] + c*weights[2];
    }

    template <typename T>
    T baryp_ptr( const T *data, float *weights )
    {
        return data[0]*weights[0] + data[1]*weights[1] + data[2]*weights[2];
    }


    template <typename T>
    T clamp( T n, T a, T b )
    {
        return std::min(b, std::max(n, a));
    }

    // int clamp( int n, int a, int b )
    // {
    //     return std::min(b, std::max(n, a));
    // }
}

