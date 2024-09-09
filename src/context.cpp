#include "context.hpp"
#include "rasterize.hpp"
#include "color.hpp"
#include "math.hpp"

#include <vector>
#include <iostream>
#include <cstring>


sven::context::context( uint32_t w, uint32_t h )
:   m_rendertarget(w, h)
{
    m_targets[COLOR_BUFFER_IDX] = sven::texture_new<uint32_t>(w, h, 4);
    m_targets[DEPTH_BUFFER_IDX] = sven::texture_new<float>(w, h, 4);

    clearColor(sven::COLOR_BUFFER_BIT, packRGB(160, 204, 254));
    clearColorf(sven::DEPTH_BUFFER_BIT, 0.0f);

    m_nativeres_surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}


void
sven::context::swapWindow( SDL_Window *win, SDL_Surface *surface )
{
    // auto &A = m_targets[DEPTH_BUFFER_IDX];
    // std::cout << "z buffer: " << ((float *)(A.pixels))[A.w*(A.h/2) + A.w/2] << "\n";

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




float
distPlanePoint( const glm::vec3 &plane, const glm::vec3 &N, const glm::vec3 &point )
{
    return glm::dot(N, point - plane);
}



void
sven::context::drawArrays( const VertexArray &buf, const glm::mat4 &T,
                           const glm::mat4 &P, const glm::mat4 &V )
{
    VaryingArray VS_buffer(buf.size());
    RenderState  RS(P, V, T);

    float dist = glm::dot(RS.front, glm::vec3(T[3]) - RS.viewpos);
    if (dist + sqrt(buf.radiusSq) < 0.0f)
    {
        return;
    }

    context::vertex_stage(buf, VS_buffer, RS);
    context::fragment_stage(VS_buffer, RS);
}


void
sven::context::vertex_stage( const VertexArray &src, VaryingArray &dst, const RenderState &RS )
{
    static VertexArray *tmp[2] = { new VertexArray, new VertexArray };

    auto &A = *tmp[0];
    auto &B = *tmp[1];

    B.clear();
    for (uint32_t i=0; i<src.size(); i+=3)
    {
        vertex_backface(src, i, B, RS);
    }

    // A.clear();
    // for (uint32_t i=0; i<B.size(); i+=3)
    // {
    //     vertex_clipping(B, i, A, RS);
    // }

    for (uint32_t i=0; i<B.size(); i+=3)
    {
        varying_stage(B, dst, i, RS);
    }
}


void
sven::context::vertex_backface( const VertexArray &src, int src_idx, VertexArray &dst,
                                const RenderState &RS )
{
    glm::vec3 A = src.pos[src_idx+0];
    glm::vec3 B = src.pos[src_idx+1];
    glm::vec3 C = src.pos[src_idx+2];
    

    glm::vec3 world = RS.T * glm::vec4(A, 1.0f);
    glm::vec3 N = glm::normalize(glm::cross(B-A, C-A));
              N = RS.T * glm::vec4(N, 0.0f);

    glm::vec3 viewdir = glm::normalize(RS.viewpos - world);

    if (glm::dot(N, viewdir) < 0.0f)
    {
        return;
    }

    int dst_idx = dst.size();
    dst.push_n(3);
    VertexArray::vertcpy(dst, src, dst_idx, src_idx, 3);
}




bool inside_frustum( const glm::vec3 pos[6], const glm::vec3 dir[6], const glm::vec3 &point )
{
    for (int i=0; i<6; i++)
    {
        float dist = glm::dot(dir[i], point - pos[i]);

        if (dist < 0.0f)
        {
            return false;
        }
    }

    return true;
}


void
sven::context::vertex_clipping( const VertexArray &src, int idx, VertexArray &dst,
                                const RenderState &RS )
{
    static constexpr float near   = 0.1f;
    static constexpr float far    = 512.0f;
    static constexpr float aspect = 1800.0f / 1250.0f;
    static constexpr float vfov   = 90.0f;

    const float halfVSide = far * tanf(vfov * 0.5f);
    const float halfHSide = halfVSide * aspect;

    glm::vec3 pos[6];
    glm::vec3 dir[6];

    {
        pos[0] = RS.viewpos + near*RS.front;
        dir[0] = RS.front;

        pos[1] = RS.viewpos + far*RS.front;
        dir[1] = -RS.front;

        pos[2] = RS.viewpos;
        dir[2] = glm::cross(far*RS.front - halfHSide*RS.right, RS.up);

        pos[3] = RS.viewpos;
        dir[3] = glm::cross(far*RS.front - halfHSide*RS.right, RS.up);

        pos[4] = RS.viewpos;
        dir[4] = glm::cross(far*RS.front + halfVSide*RS.up, RS.right);

        pos[5] = RS.viewpos;
        dir[5] = glm::cross(far*RS.front - halfVSide*RS.up, RS.right);
    }




    int num_outside = 0;

    for (int i=0; i<3; i++)
    {
        if (inside_frustum(pos, dir, src.pos[idx+i]) == false)
        {
            num_outside += 1;
        }
    }


    int dst_idx = dst.size();
    dst.push_n(3);
    VertexArray::vertcpy(dst, src, dst_idx, idx, 3);

    for (int i=0; i<3; i++)
    {
        dst.color[dst_idx+i]   = glm::vec3(0, 1, 1);
        dst.color[dst_idx+i].r = glm::min(float(num_outside) / 3.0f, 1.0f);
    }

}




void
sven::context::varying_stage( const VertexArray &src, VaryingArray &dst, int src_idx,
                              const RenderState &RS)
{
    const glm::vec3 viewpos = glm::vec3(glm::inverse(RS.V)[3]);
    const glm::vec2 size = glm::vec2(m_targets[0].w, m_targets[0].h);

    glm::vec4 object, world, view, proj;
    glm::vec3 ndc;
    glm::vec2 screen;
    float inv_w;


    // Cull if all three vertices are behind camera
    // --------------------------------------------------------------------
    int num_behind = 0;

    for (int i=0; i<3; i++)
    {
        world = RS.T * glm::vec4(glm::vec3(src.pos[src_idx+i]), 1.0f);

        float dist = glm::dot(RS.front, glm::vec3(world) - (RS.viewpos + 0.05f*RS.front));
        if (dist < 0.0f)
        {
            num_behind += 1;
        }
    }

    if (num_behind == 3)
    {
        return;
    }
    // --------------------------------------------------------------------



    int dst_idx = dst.size();
    dst.push_n(3);
    VertexArray::vertcpy(dst, src, dst_idx, src_idx, 3);

    glm::vec4 sv_Position[3] = { dst.pos[dst_idx+0], dst.pos[dst_idx+1], dst.pos[dst_idx+2] };

    for (int i=0; i<3; i++)
    {
        proj = RS.P * RS.V * RS.T * glm::vec4(glm::vec3(dst.pos[dst_idx+i]), 1.0f);

        ndc      = glm::vec3(proj) / std::max(proj.w, 0.01f);
        screen   = size * (glm::vec2(ndc) * 0.5f + 0.5f);
        screen.y = size.y - screen.y;

        dst.wpos[dst_idx+i] = RS.V * RS.T * glm::vec4(glm::vec3(dst.pos[dst_idx+i]), 1.0f);
        dst.pos[dst_idx+i]  = glm::vec4(screen, 1/std::max(ndc.z, 0.01f), 1/std::max(proj.w, 0.01f));
        dst.norm[dst_idx+i] = glm::normalize(RS.T3 * dst.norm[dst_idx+i]);
    }
}



void
sven::context::fragment_stage( const VaryingArray &buf, const RenderState &RS )
{
    for (uint32_t i=0; i<buf.size(); i+=3)
    {
        internal::rasterize(
            buf, i,
            m_targets[DEPTH_BUFFER_IDX],
            m_targets[COLOR_BUFFER_IDX],
            *m_sources[0],
            RS
        );
    }

}
