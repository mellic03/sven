#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include <SDL2/SDL.h>

#include "vertex.hpp"
#include "texture.hpp"



namespace sven
{
    class context;
    struct RenderState;
}


struct sven::RenderState
{
    glm::mat4 P = glm::mat4(1.0f);
    glm::mat4 V = glm::mat4(1.0f);
    glm::mat4 T = glm::mat4(1.0f);

    glm::mat4 PVT, PV;
    glm::mat3 VT3, V3, T3;

    glm::vec3 viewpos = glm::vec3(0);
    glm::vec3 up      = glm::vec3(0, 1, 0);
    glm::vec3 right   = glm::vec3(1, 0, 0);
    glm::vec3 front   = glm::vec3(0, 0, -1);

    RenderState() {  }

    RenderState( const glm::mat4 &p, const glm::mat4 &v, const glm::mat4 &t )
    :   P(p),
        V(v),
        T(t)
    {
        PVT = P*V*T;
        PV  = P*V;

        V3  = glm::mat3(V);
        T3  = glm::mat3(T);
        VT3 = V3*T3;

        viewpos = glm::vec3(glm::inverse(V)[3]);
        up      = glm::inverse(glm::mat3(V)) * glm::vec3(0, 1, 0);
        right   = glm::inverse(glm::mat3(V)) * glm::vec3(1, 0, 0);
        front   = glm::inverse(glm::mat3(V)) * glm::vec3(0, 0, -1);
    }

};


class sven::context
{
private:
    RenderTexture m_rendertarget;
    Texture       m_targets[2];
    Texture      *m_sources[2];
    SDL_Surface  *m_nativeres_surface;
    bool          m_cullface = true;

    void vertex_stage( const VertexArray &src, VaryingArray &dst, const RenderState& );
    void vertex_backface( const VertexArray &src, int idx, VertexArray &dst, const RenderState& );
    void vertex_clipping( const VertexArray &src, int idx, VertexArray &dst, const RenderState& );
    void varying_stage( const VertexArray &src, VaryingArray &dst, int idx, const RenderState& );
    void fragment_stage( const VaryingArray &buf, const RenderState& );

public:
         context( uint32_t w, uint32_t h );

    void swapWindow( SDL_Window *win, SDL_Surface *surface );

    void clearColor( uint32_t mask, uint32_t fill );
    void clearColorf( uint32_t mask, float fill );
    void clear( uint32_t mask );

    void bindTexture( sven::Texture &tex )
    {
        m_sources[0] = &tex;
    }

    void cullFace( bool b ) { m_cullface = b; };

    void drawArrays( const VertexArray &buf, const glm::mat4 &T,
                     const glm::mat4 &P, const glm::mat4 &V );

};


