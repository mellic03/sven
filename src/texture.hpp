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
    struct RenderTexture;

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
    glm::vec4 sample_f32( const glm::vec2 &uv, float level=0.0f );

};


struct sven::RenderTexture
{
    std::vector<uint32_t *> color_tiles;
    std::vector<float *>    depth_tiles;
    uint32_t w, h;

    RenderTexture() {  };

    RenderTexture( uint32_t width, uint32_t height )
    {
        w = width;
        h = height;

        if (w == 0 || h == 0 || w%64 != 0 || h%64 != 0)
        {
            std::cout << "w, h must be divisble by 64!\n";
            exit(1);
        }

        uint32_t grid_w = w/64;
        uint32_t grid_h = h/64;

        for (uint32_t i=0; i<grid_w*grid_h; i++)
        {
            color_tiles.push_back(new uint32_t[64*64]);
            depth_tiles.push_back(new float[64*64]);

            sven::memset32(color_tiles.back(), 0, 64*64);
            sven::memset32(depth_tiles.back(), 0, 64*64);
        }
    }
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

