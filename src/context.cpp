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
    clearColorf(sven::DEPTH_BUFFER_BIT, 1.0f);

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
    std::vector<Vertex> VS_buffer(buf.size());

    context::vertex_stage(buf, VS_buffer, T, P, V);
    context::fragment_stage(VS_buffer, V);
}


void
sven::context::vertex_stage_tri( const Vertex *src, std::vector<Vertex> &dst,
                                 const glm::mat4 &T, const glm::mat4 &P, const glm::mat4 &V )
{
    static Vertex tmp[3];
    std::memcpy(tmp, src, 3*sizeof(Vertex));


    glm::vec3 viewpos = glm::vec3(glm::inverse(V)[3]);

    const glm::vec2 size = glm::vec2(m_targets[0].w, m_targets[0].h);
    const glm::mat4 PVT  = (P * V) * T;
    const glm::mat3 T3   = glm::mat3(T);
    const glm::mat3 V3T3 = glm::mat3(V) * T3;

    glm::vec4 proj;
    glm::vec3 world;
    glm::vec3 ndc;
    glm::vec2 screen;
    float inv_z;

    for (int i=0; i<3; i++)
    {
        world    = glm::vec3(T * tmp[i].pos);
        proj     = PVT * glm::vec4(glm::vec3(tmp[i].pos), 1.0f);
        inv_z    = 1.0f / std::max(proj.w, 0.00001f);
    
        ndc      = glm::vec3(proj) * inv_z;
        screen   = size * (glm::vec2(ndc) * 0.5f + 0.5f);
        screen.y = size.y - screen.y;

        tmp[i].pos  = glm::vec4(screen, ndc.z, inv_z);
        tmp[i].norm = glm::normalize(T3 * tmp[i].norm);

        if (glm::dot(tmp[i].norm, glm::normalize(viewpos - world)) < 0.0f)
        {
            return;
        }
    }

    for (int i=0; i<3; i++)
    {
        dst.push_back(tmp[i]);
    }
    // std::memcpy(dst, tmp, 3*sizeof(Vertex));
}


void
sven::context::vertex_stage( const std::vector<Vertex> &src, std::vector<Vertex> &dst,
                             const glm::mat4 &T, const glm::mat4 &P, const glm::mat4 &V )
{
    for (uint32_t i=0; i<src.size(); i+=3)
    {
        vertex_stage_tri(&src[i], dst, T, P, V);
    }
}


void
sven::context::fragment_stage( const std::vector<Vertex> &buf, const glm::mat4 &V )
{
    for (uint32_t i=0; i<buf.size(); i+=3)
    {
        internal::rasterize(&buf[i], m_targets[DEPTH_BUFFER_IDX], m_targets[COLOR_BUFFER_IDX]);
    }
}

