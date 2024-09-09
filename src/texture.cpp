#include "texture.hpp"
#include "color.hpp"

#include <cstring>
#include <iostream>


uint32_t
sven::Texture::sample_u32( const glm::vec2 &uv, int level )
{
    uint32_t *src = (uint32_t *)(levels[level]);
    uint32_t  src_w = w >> level;
    uint32_t  src_h = h >> level;

    int x = int(uv.x * src_w) % src_w;
    int y = int(uv.y * src_h) % src_h;

    return src[src_w*y + x];
}

uint32_t
sven::Texture::sample_u32( int x, int y, int level )
{
    uint32_t *src = (uint32_t *)(levels[level]);
    uint32_t  src_w = w >> level;
    uint32_t  src_h = h >> level;

    x %= src_w;
    y %= src_h;

    return src[src_w*y + x];
}



glm::vec4
sven::Texture::nearest_f32( const glm::vec2 &uv, float level )
{
    float lo    = glm::clamp(glm::floor(level), 0.0f, float(MAX_LEVEL));
    float hi    = glm::clamp(glm::ceil(level),  0.0f, float(MAX_LEVEL));
    float alpha = level - lo;

    glm::vec4 a = unpackRGBAf(sample_u32(uv, lo));
    glm::vec4 b = unpackRGBAf(sample_u32(uv, hi));

    return glm::mix(a, b, alpha);
}



glm::vec4
sven::Texture::billinear_f32( const glm::vec2 &uv, float level )
{
    float lo    = glm::clamp(glm::floor(level), 0.0f, float(MAX_LEVEL));
    float hi    = glm::clamp(glm::ceil(level),  0.0f, float(MAX_LEVEL));
    float alpha = level - lo;
    int   mip   = glm::clamp(int(level), 0, int(MAX_LEVEL));

    uint32_t *src = (uint32_t *)(levels[mip]);
    uint32_t  src_w = (w >> mip);
    uint32_t  src_h = (h >> mip);

    glm::vec2 tsize    = glm::vec2(src_w, src_h);;
    glm::vec2 texcoord = uv + (0.5f / tsize);
    glm::vec2 texel    = texcoord * tsize;

    float u_factor = texel.x - glm::floor(texel.x);
    float v_factor = texel.y - glm::floor(texel.y);


    glm::vec4 UL   = nearest_f32((texel + glm::vec2(0, 0)) / tsize, level);
    glm::vec4 UR   = nearest_f32((texel + glm::vec2(1, 0)) / tsize, level);
    glm::vec4 ULUR = glm::mix(UL, UR, u_factor);

    glm::vec4 DL   = nearest_f32((texel + glm::vec2(0, 1)) / tsize, level);
    glm::vec4 DR   = nearest_f32((texel + glm::vec2(1, 1)) / tsize, level);
    glm::vec4 DLDR = glm::mix(DL, DR, u_factor);

    return glm::mix(ULUR, DLDR, v_factor);
}






sven::Texture
sven::texture_load( const std::string &path, bool mipmap )
{
    SDL_Surface      *tmp      = IMG_Load(path.c_str());
    SDL_PixelFormat  *target   = SDL_AllocFormat(SDL_PIXELFORMAT_BGRA32);
    SDL_Surface      *img      = SDL_ConvertSurface(tmp, target, 0);

    sven::Texture tex = {
        .w      = uint32_t(img->w),
        .h      = uint32_t(img->h),
        .bpp    = 4
    };

    tex.levels[0] = (void *)(new uint32_t[img->w * img->h]);
    uint32_t nbytes = tex.w * tex.h * sizeof(uint32_t);
    std::memcpy(tex.levels[0], img->pixels, nbytes);

    SDL_FreeFormat(target);
    SDL_FreeSurface(tmp);
    SDL_FreeSurface(img);


    if (mipmap)
    {
        texture_genmips(tex);
    }

    return tex;
}


void
sven::texture_genmips( Texture &tex )
{
    for (int i=1; i<Texture::MAX_LEVEL+1; i++)
    {
        tex.levels[i] = new uint32_t[(tex.w >> i) * (tex.h >> i)];
    }

    for (int i=1; i<Texture::MAX_LEVEL+1; i++)
    {
        uint32_t *src   = (uint32_t *)(tex.levels[i-1]);
        uint32_t  src_w = tex.w >> (i-1);
        uint32_t  src_h = tex.h >> (i-1);

        uint32_t *dst   = (uint32_t *)(tex.levels[i]);
        uint32_t  dst_w = tex.w >> i;
        uint32_t  dst_h = tex.h >> i;

        texture_downsample(src, src_w, src_h, dst, dst_w, dst_h);
    }
}



void
sven::texture_downsample( const uint32_t *src, uint32_t src_w, uint32_t src_h,
                          uint32_t *dst, uint32_t dst_w, uint32_t dst_h )
{
    using namespace glm;

    ivec2 offsets[4] = { ivec2(-1, -1), ivec2(+1, -1), ivec2(+1, +1), ivec2(-1, +1) };

    for (int y=0; y<dst_w; y++)
    {
        for (int x=0; x<dst_w; x++)
        {
            glm::vec4 result = glm::vec4(0.0f);

            for (int i=0; i<4; i++)
            {
                int row = 2*y + offsets[i].y;
                int col = 2*x + offsets[i].x;
            
                row = glm::clamp(row, 0, int(src_h)-1);
                col = glm::clamp(col, 0, int(src_w)-1);

                result += sven::unpackRGBAf(src[src_w*row + col]);
            }

            result /= 4.0f;

            dst[dst_w*y + x] = sven::packRGBAf(result);
        }
        
    }
}





void
sven::texture_clear( Texture &tex )
{
    uint32_t *buf  = (uint32_t *)(tex.levels[0]);
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
    std::memcpy(dst->pixels, src.levels[0], nbytes);
}

