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
}


class sven::context
{
private:
    Texture m_targets[2];
    SDL_Surface *m_nativeres_surface;

    void vertex_stage( const std::vector<Vertex> &src, std::vector<Vertex> &dst,
                       const glm::mat4 &T, const glm::mat4 &P, const glm::mat4 &V );

    /**
     * @param buf Vertex data. buf.size() be divisible by 3.
    */
    void fragment_stage( const std::vector<Vertex> &buf );

public:
         context( uint32_t w, uint32_t h );

    void swapWindow( SDL_Window *win, SDL_Surface *surface );

    void clearColor( uint32_t mask, uint32_t fill );
    void clearColorf( uint32_t mask, float fill );
    void clear( uint32_t mask );
    void drawArrays( const std::vector<Vertex> &buf, const glm::mat4 &T,
                     const glm::mat4 &P, const glm::mat4 &V );

};


