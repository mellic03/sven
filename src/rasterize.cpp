#include "rasterize.hpp"
#include "context.hpp"
#include "math.hpp"
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
BarData_load( const sven::Vertex *vertices )
{
    BarData d;

    d.v1x = vertices[0].pos.x, d.v1y = vertices[0].pos.y;
    d.v2x = vertices[1].pos.x, d.v2y = vertices[1].pos.y;
    d.v3x = vertices[2].pos.x, d.v3y = vertices[2].pos.y;

    d.denom = (d.v2y-d.v3y)*(d.v1x-d.v3x) + (d.v3x-d.v2x)*(d.v1y-d.v3y);

    return d;
}
static BarData
BarData_load2( const glm::vec4 pos[4] )
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




void sven::internal::rasterize( const Vertex *vertices, Texture &dst_depth, Texture &dst_color )
{
    using namespace glm;

    static Vertex buf[3];
    std::memcpy(buf, vertices, 3*sizeof(Vertex));

    vec3  color[3] = { vec3(255, 0, 0), vec3(0, 255, 0), vec3(0, 0, 255) };
    vec3  norm[3];
    float inv_z[3] = { buf[0].pos.w,    buf[1].pos.w,    buf[2].pos.w    };

    for (int i=0; i<3; i++)
    {
        norm[i]   = buf[i].norm * inv_z[i];
        color[i] *= inv_z[i];
    }

    float boundsf[4] = {+INFINITY, -INFINITY, +INFINITY, -INFINITY};
    int   boundsi[4];
    float bWeights[4];

    // for (int i=0; i<3; i++)
    // {
    //     if (buf[i].pos.w <= 0.0f)
    //     {
    //         return;
    //     }
    // }

    for (int i=0; i<3; i++)
    {
        boundsf[0] = std::min(boundsf[0], buf[i].pos.x);
        boundsf[1] = std::max(boundsf[1], buf[i].pos.x);
        boundsf[2] = std::min(boundsf[2], buf[i].pos.y);
        boundsf[3] = std::max(boundsf[3], buf[i].pos.y);
    }

    uint32_t *pixel_buf = (uint32_t *)(dst_color.pixels);
    float    *depth_buf = (float *)(dst_depth.pixels);
    int       w = dst_color.w;
    int       h = dst_color.h;

    int xmin = sven::clamp(int(boundsf[0]), 0, w-1);
    int xmax = sven::clamp(int(boundsf[1]), 0, w-1);
    int ymin = sven::clamp(int(boundsf[2]), 0, h-1);
    int ymax = sven::clamp(int(boundsf[3]), 0, h-1);

    if (xmin == xmax || ymin == ymax)
    {
        return;
    }


    const BarData bData = BarData_load(buf);
    uint32_t bWeightsi[4];

    vec4 sv_FragCoord;

    for (int y=ymin; y<=ymax; y++)
    {
        for (int x=xmin; x<=xmax; x++)
        {
            barycentric2D(x, y, bData, bWeights);
            std::memcpy(bWeightsi, bWeights, 4*sizeof(float));

            if ((bWeightsi[0] | bWeightsi[1] | bWeightsi[2]) & 0x80000000)
            // if (bWeights[0] < 0.05f || bWeights[1] < 0.05f || bWeights[2] < 0.05f)
            {
                continue;
            }

            sv_FragCoord.x = x;
            sv_FragCoord.y = y;
            sv_FragCoord.z = sven::baryp(buf[0].pos.z, buf[1].pos.z, buf[2].pos.z, bWeights);
            sv_FragCoord.w = 1.0f / sven::baryp(inv_z[0], inv_z[1], inv_z[2], bWeights);

            // std::cout << sv_FragCoord.z << ",\t" << sv_FragCoord.w << "\n";

            if (sv_FragCoord.z < 0.0f || 1.0f < sv_FragCoord.z)
            {
                continue;
            }

            int idx = w*y + x;
    
            if (sv_FragCoord.z < depth_buf[idx])
            {
                depth_buf[idx] = sv_FragCoord.z;

                // vec3 C  = sven::baryp(color[0], color[1], color[2], bWeights);
                //      C *= sv_FragCoord.w;
                // uint32_t color = packRGB(C.r, C.g, C.b);

                float d = std::min(bWeights[0], std::min(bWeights[1], bWeights[2]));
                      d = 1 - (1-d)*(1-d);
                      d = (d > 0.1f) ? 1.0f : 0.0f;

                vec3 N  = sven::baryp(norm[0], norm[1], norm[2], bWeights);
                     N *= sv_FragCoord.w;
                     N  = d * (normalize(N) * 0.5f + 0.5f);

                vec3 L = normalize(vec3(1));
                N = vec3(1) * max(dot(N, L), 0.0f);
                N = 255.0f *  clamp(N, 0.0f, 1.0f);

                uint32_t color = packRGB(N.x, N.y, N.z);

                pixel_buf[idx] = color;
            }
        }
    }
}


void sven::internal::rasterize( const VaryingArray &buf, int vidx,
                                Texture &dst_depth, Texture &dst_color )
{
    using namespace glm;

    static vec4 pos[3];
    static vec3 norm[3];
    static vec2 uv[3];

    std::memcpy(pos,  &buf.pos[vidx],  sizeof(pos));
    std::memcpy(norm, &buf.norm[vidx], sizeof(norm));
    std::memcpy(uv,   &buf.uv[vidx],   sizeof(uv));

    vec3  color[3] = { vec3(255, 0, 0), vec3(0, 255, 0), vec3(0, 0, 255) };
    float inv_z[3] = { pos[0].w, pos[1].w, pos[2].w };

    for (int i=0; i<3; i++)
    {
        norm[i]  *= inv_z[i];
        color[i] *= inv_z[i];
    }

    float boundsf[4] = {+INFINITY, -INFINITY, +INFINITY, -INFINITY};
    int   boundsi[4];
    float bWeights[4];

    for (int i=0; i<3; i++)
    {
        boundsf[0] = std::min(boundsf[0], pos[i].x);
        boundsf[1] = std::max(boundsf[1], pos[i].x);
        boundsf[2] = std::min(boundsf[2], pos[i].y);
        boundsf[3] = std::max(boundsf[3], pos[i].y);
    }

    uint32_t *pixel_buf = (uint32_t *)(dst_color.pixels);
    float    *depth_buf = (float *)(dst_depth.pixels);
    int       w = dst_color.w;
    int       h = dst_color.h;

    int xmin = sven::clamp(int(boundsf[0]), 0, w-1);
    int xmax = sven::clamp(int(boundsf[1]), 0, w-1);
    int ymin = sven::clamp(int(boundsf[2]), 0, h-1);
    int ymax = sven::clamp(int(boundsf[3]), 0, h-1);

    if (xmin == xmax || ymin == ymax)
    {
        return;
    }


    const BarData bData = BarData_load2(pos);
    uint32_t bWeightsi[4];

    vec4 sv_FragCoord;

    for (int y=ymin; y<=ymax; y++)
    {
        for (int x=xmin; x<=xmax; x++)
        {
            barycentric2D(x, y, bData, bWeights);
            std::memcpy(bWeightsi, bWeights, 4*sizeof(float));

            if ((bWeightsi[0] | bWeightsi[1] | bWeightsi[2]) & 0x80000000)
            {
                continue;
            }

            sv_FragCoord.x = x;
            sv_FragCoord.y = y;
            sv_FragCoord.z = sven::baryp(pos[0].z, pos[1].z, pos[2].z, bWeights);
            sv_FragCoord.w = 1.0f / sven::baryp(inv_z[0], inv_z[1], inv_z[2], bWeights);

            if (sv_FragCoord.z < 0.0f || 1.0f < sv_FragCoord.z)
            {
                continue;
            }

            int idx = w*y + x;
    
            if (sv_FragCoord.z < depth_buf[idx])
            {
                depth_buf[idx] = sv_FragCoord.z;

                float d = std::min(bWeights[0], std::min(bWeights[1], bWeights[2]));
                      d = 1 - (1-d)*(1-d);
                      d = (d > 0.1f) ? 1.0f : 0.0f;

                vec3 N  = sven::baryp(norm[0], norm[1], norm[2], bWeights);
                     N *= sv_FragCoord.w;
                     N  = d * (normalize(N) * 0.5f + 0.5f);

                vec3 L = normalize(vec3(1));
                N = vec3(1) * max(dot(N, L), 0.0f);
                N = 255.0f *  clamp(N, 0.0f, 1.0f);

                uint32_t color = packRGB(N.x, N.y, N.z);

                pixel_buf[idx] = color;
            }
        }
    }
}



// void sven::quad( float x, float y, float w, float h, float theta, uint32_t fill )
// {
//     float hw = w / 2.0f;
//     float hh = h / 2.0f;

//     static std::vector<Vertex> buf(6);

//     buf[0].pos = glm::vec4(-hw, -hh, 0.0f, 1.0f);
//     buf[1].pos = glm::vec4(+hw, -hh, 0.0f, 1.0f);
//     buf[2].pos = glm::vec4(+hw, +hh, 0.0f, 1.0f);

//     buf[3].pos = glm::vec4(-hw, -hh, 0.0f, 1.0f);
//     buf[4].pos = glm::vec4(-hw, +hh, 0.0f, 1.0f);
//     buf[5].pos = glm::vec4(+hw, +hh, 0.0f, 1.0f);

//     for (int i=0; i<6; i++)
//     {
//         buf[i].pos = sven::vec4_rotate_xy(buf[i].pos, theta);

//         buf[i].pos.x += x;
//         buf[i].pos.y += y;
//     }

//     internal::fragment_stage(buf);
// }


uint32_t
sven::packR( uint32_t r )
{
    return packRGBA(r, r, r, r);
}

uint32_t
sven::packRGB( uint32_t r, uint32_t g, uint32_t b )
{
    return packRGBA(r, g, b, 255);
}

uint32_t
sven::packRGBA( uint32_t r, uint32_t g, uint32_t b, uint32_t a )
{
    return (a << 24) + (r << 16) + (g << 8) + (b << 0);
}
