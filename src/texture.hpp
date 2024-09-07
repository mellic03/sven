#pragma once

#include <cstdint>
#include <cstddef>

#include <SDL2/SDL.h>


namespace sven
{
    struct Texture;

    Texture texture_new( uint32_t w, uint32_t h, uint32_t bpp );
    void texture_clear( Texture& );
    void texture_blit( const Texture &src, SDL_Surface *dst );
    void surface_blit( SDL_Surface *src, SDL_Surface *dst );
}


struct sven::Texture
{
    uint8_t  *pixels;
    uint32_t  w, h, bpp;
    uint32_t  fill = 0;

    // Texture() {  };

    // Texture( uint32_t width, uint32_t height, uint32_t bitspp )
    // :   w(width),
    //     h(height),
    //     bpp(bitspp),
    //     pixels(new uint8_t[width*height*bitspp])
    // {
    //     sven::texture_clear(this, 0);
    // }

};


