#include "rasterize.hpp"
#include "context.hpp"
#include "math.hpp"
#include "color.hpp"
#include "memory.hpp"

#include <immintrin.h>
#include <iostream>

#include <vector>
#include <cstring>



struct BarData
{
    float v1x, v1y, v2x, v2y, v3x, v3y;
    float denom;
};


static BarData
BarData_load( const glm::vec4 pos[3] )
{
    BarData d;

    d.v1x = pos[0].x, d.v1y = pos[0].y;
    d.v2x = pos[1].x, d.v2y = pos[1].y;
    d.v3x = pos[2].x, d.v3y = pos[2].y;

    d.denom = (d.v2y-d.v3y)*(d.v1x-d.v3x) + (d.v3x-d.v2x)*(d.v1y-d.v3y);

    return d;
}


static void
barycentric2D( float x, float y, const BarData &d, float *weights )
{
    weights[0] = ((d.v2y-d.v3y)*(x-d.v3x) + (d.v3x-d.v2x)*(y-d.v3y)) / d.denom;
    weights[1] = ((d.v3y-d.v1y)*(x-d.v3x) + (d.v1x-d.v3x)*(y-d.v3y)) / d.denom;
    weights[2] = 1.0f - weights[0] - weights[1];
}


template <typename T>
T inv_baryp( const T *data, float *invz, float *weights )
{
    T result  = sven::baryp(data[0]*invz[0], data[1]*invz[1], data[2]*invz[2], weights);
      result /= sven::baryp(invz[0], invz[1], invz[2], weights);
    return result;
}


template <typename T>
T do_thing2( T *data, float *invz, float *weights )
{
    T ree  = sven::baryp(data[0], data[1], data[2], weights);
      ree /= sven::baryp(invz[0], invz[1], invz[2], weights);
    return ree;
}


float computeMipLevel( int x, int y, glm::vec3 wpos[3], float weights[3] )
{
    static constexpr float MAX_LEVEL = float(sven::Texture::MAX_LEVEL);

    float z_view   = -sven::baryp(wpos[0], wpos[1], wpos[2], weights).z;
    float miplevel = MAX_LEVEL * (z_view / 16.0f);

    return miplevel;
}


// void sven::internal::rasterize( const VaryingArray &buf, int vidx,
//                                 Texture &dst_depth, Texture &dst_color, Texture &src_color,
//                                 const RenderState &RS )
// {
//     using namespace glm;

//     static vec4 pos[3];
//     static vec3 wpos[3];
//     static vec3 norm[3];
//     static vec2 uv[3];

//     std::memcpy(pos,  &buf.pos[vidx],  sizeof(pos));
//     std::memcpy(wpos, &buf.wpos[vidx], sizeof(wpos));
//     std::memcpy(norm, &buf.norm[vidx], sizeof(norm));
//     std::memcpy(uv,   &buf.uv[vidx],   sizeof(uv));

//     vec2  dst_size = vec2(dst_color.w, dst_color.h);
//     float inv_z[3] = { pos[0].z, pos[1].z, pos[2].z };
//     float inv_w[3] = { pos[0].w, pos[1].w, pos[2].w };

//     for (int i=0; i<3; i++)
//     {
//         norm[i]  *= inv_w[i];
//         uv[i]    *= inv_w[i];
//     }

//     uint32_t *pixel_buf = (uint32_t *)(dst_color.levels[0]);
//     float    *depth_buf = (float *)(dst_depth.levels[0]);
//     int       w = dst_color.w;
//     int       h = dst_color.h;

//     int xmin = sven::clamp(int(std::min(pos[0].x, std::min(pos[1].x, pos[2].x))), 0, w-1);
//     int xmax = sven::clamp(int(std::max(pos[0].x, std::max(pos[1].x, pos[2].x))), 0, w-1);
//     int ymin = sven::clamp(int(std::min(pos[0].y, std::min(pos[1].y, pos[2].y))), 0, h-1);
//     int ymax = sven::clamp(int(std::max(pos[0].y, std::max(pos[1].y, pos[2].y))), 0, h-1);

//     if (xmin == xmax || ymin == ymax)
//     {
//         return;
//     }

//     BarData  bData = BarData_load(pos);
//     float    weights[3];

//     vec4 sv_FragCoord;

//     for (int y=ymin; y<=ymax; y++)
//     {
//         for (int x=xmin; x<=xmax; x++)
//         {
//             // if (y%32 == 0 || x%32 == 0)
//             // {
//             //     pixel_buf[w*y + x] = 0;
//             //     depth_buf[w*y + x] = 1.0f;
//             //     continue;
//             // }

//             barycentric2D(x, y, bData, weights);

//             if (weights[0] < 0.0f || weights[1] < 0.0f || weights[2] < 0.0f)
//             {
//                 continue;
//             }

//             sv_FragCoord.x = x;
//             sv_FragCoord.y = y;
//             sv_FragCoord.z = 1.0f / sven::baryp_ptr(inv_z, weights);
//             sv_FragCoord.w = 1.0f / sven::baryp_ptr(inv_w, weights);

//             if (sv_FragCoord.z <= 0.0f)
//             {
//                 continue;
//             }

//             int idx = w*y + x;
    
//             if (1/sv_FragCoord.z > depth_buf[idx])
//             {
//                 depth_buf[idx] = 1/sv_FragCoord.z;

//                 vec3 N = sv_FragCoord.w * sven::baryp_ptr(norm, weights);
//                           N = normalize(N);

//                 vec2  texcoord = sv_FragCoord.w * sven::baryp_ptr(uv, weights);
//                 float miplevel = computeMipLevel(x, y, wpos, weights);
//                 vec3  color    = src_color.nearest_f32(texcoord, miplevel);


//                 pixel_buf[idx] = packRGBf(color);
//             }
//         }
//     }
// }
