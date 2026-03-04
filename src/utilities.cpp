#include "utilities.hpp"

int line_height(TTF_Font *font)
{
    const char *testLine = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()";
    int h;
    int w;
    TTF_SizeText(font, testLine, &w, &h);
    return h;
}

RasterizedTextInfo::RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, const char *text, SDL_Color color)
{
    TTF_SizeText(font, text, &this->w, &this->h);
    SDL_Surface *title_s = TTF_RenderText_Blended(font, text, color);
    this->tx = SDL_CreateTextureFromSurface(renderer, title_s);
    SDL_FreeSurface(title_s);
}

RasterizedTextInfo::~RasterizedTextInfo()
{
    SDL_DestroyTexture(this->tx);
}
