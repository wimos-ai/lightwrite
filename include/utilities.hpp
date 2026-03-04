#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int line_height(TTF_Font *font);

class RasterizedTextInfo
{
public:
    RasterizedTextInfo(TTF_Font *font, SDL_Renderer* renderer, const char *text, SDL_Color color);
    ~RasterizedTextInfo();

public:
    SDL_Texture *tx;
    int w;
    int h;
};