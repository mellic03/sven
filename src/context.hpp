#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <thread>

#include <SDL2/SDL.h>

#include "vertex.hpp"
#include "texture.hpp"
#include "rendertarget.hpp"



namespace sven
{
    class context;

    sven::context *getCurrentContext();
    void setCurrentContext( sven::context* );
}



class sven::context
{
private:
    // std::vector<std::thread> m_threads;

    vertex_binner  m_binner;
    Texture       *m_textures[2];
    SDL_Surface   *m_nativeres_surface;

public:
    context( uint32_t w, uint32_t h )
    :   m_binner(w, h)
    {
        
    }

    void swapWindow( SDL_Window*, SDL_Surface* );
};



// struct sven::RenderState
// {
//     glm::mat4 P = glm::mat4(1.0f);
//     glm::mat4 V = glm::mat4(1.0f);
//     glm::mat4 T = glm::mat4(1.0f);

//     glm::mat4 PVT, PV;
//     glm::mat3 VT3, V3, T3;

//     glm::vec3 viewpos = glm::vec3(0);
//     glm::vec3 up      = glm::vec3(0, 1, 0);
//     glm::vec3 right   = glm::vec3(1, 0, 0);
//     glm::vec3 front   = glm::vec3(0, 0, -1);

//     RenderState() {  }

//     RenderState( const glm::mat4 &p, const glm::mat4 &v, const glm::mat4 &t )
//     :   P(p),
//         V(v),
//         T(t)
//     {
//         PVT = P*V*T;
//         PV  = P*V;

//         V3  = glm::mat3(V);
//         T3  = glm::mat3(T);
//         VT3 = V3*T3;

//         viewpos = glm::vec3(glm::inverse(V)[3]);
//         up      = glm::inverse(glm::mat3(V)) * glm::vec3(0, 1, 0);
//         right   = glm::inverse(glm::mat3(V)) * glm::vec3(1, 0, 0);
//         front   = glm::inverse(glm::mat3(V)) * glm::vec3(0, 0, -1);
//     }

// };
