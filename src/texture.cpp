#include "texture.hpp"
#include "memory.hpp"

#include <cstring>


sven::Texture
sven::texture_new( uint32_t w, uint32_t h, uint32_t bpp )
{
    sven::Texture tex = {
        .pixels = new uint8_t[w*h*bpp],
        .w      = w,
        .h      = h,
        .bpp    = bpp
    };

    return tex;
}


void
sven::texture_clear( Texture &tex )
{
    uint32_t *buf  = (uint32_t *)(tex.pixels);
    uint32_t  fill = tex.fill;
    uint32_t  w    = tex.w;
    uint32_t  h    = tex.h;

    for (uint32_t y=0; y<h; y++)
    {
        sven::memset32(&(buf[w*y + 0]), fill, w);
    }

    // SDL_FillRect(tex->surface, NULL, fill);

    // uint32_t nbytes = tex->w * tex->h * tex->bpp;
    // std::memset(tex->pixels, 0, nbytes);

}


void
sven::surface_blit( SDL_Surface *src, SDL_Surface *dst )
{
    SDL_Rect r0;
    r0.x = 0;
    r0.y = 0;
    r0.w = src->w;
    r0.h = src->h;

    SDL_Rect r1;
    r1.x = 0;
    r1.y = 0;
    r1.w = dst->w;
    r1.h = dst->h;

    SDL_BlitScaled(src, &r0, dst, &r1);
}


void
sven::texture_blit( const Texture &src, SDL_Surface *dst )
{
    uint32_t nbytes = src.w * src.h * src.bpp;
    std::memcpy(dst->pixels, src.pixels, nbytes);
}

