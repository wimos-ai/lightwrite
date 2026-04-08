#include "utilities.hpp"
#include "logger.hpp"

#include <array>
#include <cstring>

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

RasterizedTextInfo::RasterizedTextInfo(TTF_Font *font, SDL_Renderer *renderer, std::string_view sv, SDL_Color color) : RasterizedTextInfo(font, renderer, RasterizedTextInfo::sv_fwd_helper(sv), color)
{
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

const char *RasterizedTextInfo::sv_fwd_helper(std::string_view sv)
{
    // HERE BE DRAGONS!
    static thread_local char *chars;
    static thread_local size_t char_sz;

    if (chars == nullptr)
    {
        chars = (char *)std::malloc(sv.size() + 1);
        char_sz = sv.size() + 1;
    }

    if (char_sz < sv.size() + 1)
    {
        std::memset(chars, 0, char_sz);
        char_sz = sv.size() + 1;
        chars = (char *)std::realloc(chars, char_sz);
    }
    memcpy(chars, sv.data(), sv.size());
    chars[sv.size()] = 0;
    return chars;
}
