#include "utilities.hpp"
#include "logger.hpp"

SDL_Color get_color_negitive(SDL_Color other)
{
    return SDL_Color{255 - other.r, 255 - other.g, 255 - other.b, other.a};
}

RasterizedTextInfo::RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, const char *text, SDL_Color color)
{

    LOG_TTF_ERROR(TTF_SizeUTF8(font, text, &this->w, &this->h));
    SDL_Surface *title_s = TTF_RenderUTF8_Blended(font, text, color);
    if (!title_s)
    {
        LOG_ERROR("TTF_RenderUTF8_Blended: %s", TTF_GetError());
    }

    this->tx = SDL_CreateTextureFromSurface(renderer, title_s);
    if (!tx)
    {
        LOG_ERROR("SDL_CreateTextureFromSurface: %s", TTF_GetError());
    }

    SDL_FreeSurface(title_s);
}

RasterizedTextInfo::~RasterizedTextInfo()
{
    SDL_DestroyTexture(this->tx);
}

void RasterizedTextInfo::render(SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect r{x, y, this->w, this->h};
    LOG_SDL_ERROR(SDL_RenderCopy(renderer, this->tx, nullptr, &r));
}