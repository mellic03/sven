#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "memory.hpp"



namespace sven
{
    struct Texture;

    template <typename T>
    Texture texture_new( uint32_t w, uint32_t h, uint32_t bpp );

    Texture texture_load( const std::string &path, bool mipmap=true );
    void    texture_downsample( const uint32_t *src, uint32_t src_w, uint32_t src_h,
                                uint32_t *dst, uint32_t dst_w, uint32_t dst_h );
    void    texture_genmips( Texture &tex );

    void texture_clear( Texture& );
    void texture_blit( const Texture &src, SDL_Surface *dst );
    void surface_blit( SDL_Surface *src, SDL_Surface *dst );
}


struct sven::Texture
{
    static constexpr int MAX_LEVEL = 7;

    void     *levels[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    uint32_t  w, h, bpp;
    uint32_t  fill = 0;

    uint32_t  sample_u32( const glm::vec2 &uv, int   level=0 );
    uint32_t  sample_u32( int x, int y, int level=0 );

    glm::vec4 nearest_f32( const glm::vec2 &uv, float level=0.0f );
    glm::vec4 billinear_f32( const glm::vec2 &uv, float level=0.0f );


};



template <typename T>
sven::Texture
sven::texture_new( uint32_t w, uint32_t h, uint32_t bpp )
{
    sven::Texture tex = {
        .w      = w,
        .h      = h,
        .bpp    = bpp
    };

    tex.levels[0] = (void *)(new T[w*h]);

    return tex;
}

