#include "utilities.hpp"

int line_height(TTF_Font *font)
{
    const char *testLine = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()";
    int h;
    int w;
    TTF_SizeText(font, testLine, &w, &h);
    return h;
}

SDL_Color get_color_negitive(SDL_Color other)
{
    return SDL_Color{255 - other.r, 255 - other.g, 255 - other.b, other.a};
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

void RasterizedTextInfo::render(SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect r{x, y, this->w, this->h};
    SDL_RenderCopy(renderer, this->tx, nullptr, &r);
}
