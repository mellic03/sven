#include "context.hpp"
#include "rasterize.hpp"
#include "color.hpp"
#include "math.hpp"

#include <vector>
#include <iostream>
#include <cstring>



namespace
{
    sven::context *CURRENT_CTX = nullptr;
}

sven::context *sven::getCurrentContext()
{
    return CURRENT_CTX;
}

void sven::setCurrentContext( sven::context *ctx )
{
    CURRENT_CTX = ctx;
}




void
sven::context::swapWindow( SDL_Window *win, SDL_Surface *surface )
{
    // sven::texture_blit(m_targets[COLOR_BUFFER_IDX], m_nativeres_surface);
    // sven::surface_blit(m_nativeres_surface, surface);

    SDL_UpdateWindowSurface(win);
}


// void
// sven::context::clearColor( uint32_t mask, uint32_t fill )
// {
//     for (int i=0; i<2; i++)
//     {
//         if (mask & (1 << i))
//         {
//             m_targets[i].fill = fill;
//         }
//     }
// }


// void
// sven::context::clearColorf( uint32_t mask, float fill )
// {
//     uint32_t temp;
//     std::memcpy(&temp, &fill, sizeof(float));

//     for (int i=0; i<2; i++)
//     {
//         if (mask & (1 << i))
//         {
//             m_targets[i].fill = temp;
//         }
//     }
// }


// void
// sven::context::clear( uint32_t mask )
// {
//     for (int i=0; i<2; i++)
//     {
//         if (mask & (1 << i))
//         {
//             texture_clear(m_targets[i]);
//         }
//     }
// }



// float
// distPlanePoint( const glm::vec3 &plane, const glm::vec3 &N, const glm::vec3 &point )
// {
//     return glm::dot(N, point - plane);
// }



// void
// sven::context::varying_stage( const VertexArray &src, VaryingArray &dst, int src_idx,
//                               const RenderState &RS)
// {
//     const glm::vec3 viewpos = glm::vec3(glm::inverse(RS.V)[3]);
//     const glm::vec2 size = glm::vec2(m_targets[0].w, m_targets[0].h);

//     glm::vec4 object, world, view, proj;
//     glm::vec3 ndc;
//     glm::vec2 screen;
//     float inv_w;


//     // Cull if all three vertices are behind camera
//     // --------------------------------------------------------------------
//     int num_behind = 0;

//     for (int i=0; i<3; i++)
//     {
//         world = RS.T * glm::vec4(glm::vec3(src.pos[src_idx+i]), 1.0f);

//         float dist = glm::dot(RS.front, glm::vec3(world) - (RS.viewpos + 0.05f*RS.front));
//         if (dist < 0.0f)
//         {
//             num_behind += 1;
//         }
//     }

//     if (num_behind == 3)
//     {
//         return;
//     }
//     // --------------------------------------------------------------------



//     int dst_idx = dst.size();
//     dst.push_n(3);
//     VertexArray::vertcpy(dst, src, dst_idx, src_idx, 3);

//     glm::vec4 sv_Position[3] = { dst.pos[dst_idx+0], dst.pos[dst_idx+1], dst.pos[dst_idx+2] };

//     for (int i=0; i<3; i++)
//     {
//         proj = RS.P * RS.V * RS.T * glm::vec4(glm::vec3(dst.pos[dst_idx+i]), 1.0f);

//         ndc      = glm::vec3(proj) / std::max(proj.w, 0.01f);
//         screen   = size * (glm::vec2(ndc) * 0.5f + 0.5f);
//         screen.y = size.y - screen.y;

//         dst.wpos[dst_idx+i] = RS.V * RS.T * glm::vec4(glm::vec3(dst.pos[dst_idx+i]), 1.0f);
//         dst.pos[dst_idx+i]  = glm::vec4(screen, 1/std::max(ndc.z, 0.01f), 1/std::max(proj.w, 0.01f));
//         dst.norm[dst_idx+i] = glm::normalize(RS.T3 * dst.norm[dst_idx+i]);
//     }
// }



// void
// sven::context::fragment_stage( const VaryingArray &buf, const RenderState &RS )
// {
//     for (uint32_t i=0; i<buf.size(); i+=3)
//     {
//         internal::rasterize(
//             buf, i,
//             m_targets[DEPTH_BUFFER_IDX],
//             m_targets[COLOR_BUFFER_IDX],
//             *m_sources[0],
//             RS
//         );
//     }

// }
