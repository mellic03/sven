#include "context.hpp"
#include "rasterize.hpp"
#include "math.hpp"

#include <vector>
#include <iostream>
#include <cstring>


sven::context::context( uint32_t w, uint32_t h )
{
    m_targets[0] = sven::texture_new(w, h, 4);
    m_targets[1] = sven::texture_new(w, h, 4);

    clearColor(sven::COLOR_BUFFER_BIT, 0);
    clearColorf(sven::DEPTH_BUFFER_BIT, 0.0f);

    m_nativeres_surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}


void
sven::context::swapWindow( SDL_Window *win, SDL_Surface *surface )
{
    sven::texture_blit(m_targets[COLOR_BUFFER_IDX], m_nativeres_surface);
    sven::surface_blit(m_nativeres_surface, surface);

    SDL_UpdateWindowSurface(win);
}


void
sven::context::clearColor( uint32_t mask, uint32_t fill )
{
    for (int i=0; i<2; i++)
    {
        if (mask & (1 << i))
        {
            m_targets[i].fill = fill;
        }
    }
}


void
sven::context::clearColorf( uint32_t mask, float fill )
{
    uint32_t temp;
    std::memcpy(&temp, &fill, sizeof(float));

    for (int i=0; i<2; i++)
    {
        if (mask & (1 << i))
        {
            m_targets[i].fill = temp;
        }
    }
}


void
sven::context::clear( uint32_t mask )
{
    for (int i=0; i<2; i++)
    {
        if (mask & (1 << i))
        {
            texture_clear(m_targets[i]);
        }
    }
}



void
sven::context::drawArrays( const std::vector<Vertex> &buf, const glm::mat4 &T,
                           const glm::mat4 &P, const glm::mat4 &V )
{
    static std::vector<Vertex> VS_buffer(1024);

    context::vertex_stage(buf, VS_buffer, T, P, V);
    context::fragment_stage(VS_buffer);
}


void
sven::context::vertex_stage( const std::vector<Vertex> &src, std::vector<Vertex> &dst,
                              const glm::mat4 &T, const glm::mat4 &P, const glm::mat4 &V )
{
    const glm::vec2 size = glm::vec2(m_targets[0].w, m_targets[0].h);
    const glm::mat4 PV   = P * V;
    const glm::mat4 PVT  = PV * T;
    const glm::mat4 VT   = V * T;
    const glm::mat3 T3   = glm::mat3(T);
    const glm::mat3 V3   = glm::mat3(V);
    const glm::mat3 V3T3 = V3 * T3;

    glm::vec4 view, proj;
    glm::vec3 ndc;
    glm::vec2 screen;

    for (uint32_t i=0; i<src.size(); i++)
    {
        view     = VT * glm::vec4(glm::vec3(src[i].pos), 1.0f);
        proj     = PVT * glm::vec4(glm::vec3(src[i].pos), 1.0f);
        ndc      = glm::vec3(proj) / std::max(proj.w, 0.00001f);
        screen   = size * (glm::vec2(ndc) * 0.5f + 0.5f);
        screen.y = size.y - screen.y;

        dst[i].pos  = glm::vec4(screen, ndc.z, proj.w);
        dst[i].norm = glm::normalize(T3 * src[i].norm);
    }

    // static Vertex temp[3];
    // uint32_t cursor = 0;

    // for (uint32_t i=0; i<src.size(); i+=3)
    // {
    //     for (uint32_t j=0; j<3; j++)
    //     {
    //         proj     = PV * T * glm::vec4(glm::vec3(src[i+j].pos), 1.0f);
    //         ndc      = glm::vec3(proj) / proj.w;
    //         screen   = size * (glm::vec2(ndc) * 0.5f + 0.5f);
    //         screen.y = size.y - screen.y;

    //         temp[j].pos  = glm::vec4(screen, ndc.z, 1.0f);
    //         temp[j].norm = T3 * src[i].norm;
    //     }

    //     int verts_inside = 0;

    //     for (uint32_t j=0; j<3; j++)
    //     {
    //         bool x_inside = sven::clamp(temp[j].pos.x, 0.0f, size.x-1.0f) == temp[j].pos.x;
    //         bool y_inside = sven::clamp(temp[j].pos.y, 0.0f, size.y-1.0f) == temp[j].pos.y;

    //         if (x_inside && y_inside)
    //         {
    //             verts_inside += 1;
    //         }
    //     }

    //     if (verts_inside > 0)
    //     {
    //         dst[cursor+0] = temp[0];
    //         dst[cursor+1] = temp[1];
    //         dst[cursor+2] = temp[2];
    //         cursor += 3;
    //     }
    // }
}


void
sven::context::fragment_stage( const std::vector<Vertex> &buf )
{
    bool skip;

    for (uint32_t i=0; i<buf.size(); i+=3)
    {
        // skip = false;

        // for (int j=0; j<3; j++)
        // {
        //     if (glm::dot(buf[i+j].norm, glm::vec3(0, 0, 1)) < 0.0f)
        //     {
        //         skip = true;
        //         break;
        //     }
        // }

        // if (skip == false)
        // {
            internal::rasterize(&(buf[i]), m_targets[DEPTH_BUFFER_IDX], m_targets[COLOR_BUFFER_IDX]);
        // }
    }
}

