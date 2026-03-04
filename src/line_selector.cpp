#include "line_selector.hpp"
#include "keybinds.h"

LineSelector::LineSelector(const char *title_font, int title_sz, const char *text_font, int text_sz, const char *title) : title_font(TTF_OpenFont(title_font, title_sz)),
                                                                                                                          text_font(TTF_OpenFont(text_font, text_sz)),
                                                                                                                          title_height(generate_line_height(this->title_font)),
                                                                                                                          text_height(generate_line_height(this->text_font)),
                                                                                                                          title(title)
{
}

LineSelector::~LineSelector()
{
    TTF_CloseFont(title_font);
    TTF_CloseFont(text_font);
}

int LineSelector::generate_line_height(TTF_Font *font)
{
    const char *testLine = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()";
    int h;
    int w;
    TTF_SizeText(font, testLine, &w, &h);
    return h;
}

void LineSelector::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    int title_width{0};
    int title_height{0};
    TTF_SizeText(title_font, title.c_str(), &title_width, &title_height);
    SDL_Texture *title_tx = [this](SDL_Renderer *renderer)
    {
        SDL_Surface *title_s = TTF_RenderText_Blended(title_font, title.c_str(), title_color);
        auto a = SDL_CreateTextureFromSurface(renderer, title_s);
        SDL_FreeSurface(title_s);
        return a;
    }(renderer);

    int text_width{0};
    int text_height{0};
    TTF_SizeText(text_font, text.c_str(), &text_width, &text_height);
    SDL_Texture *text_tx = [this](SDL_Renderer *renderer)
    {
        SDL_Surface *text_s = TTF_RenderText_Blended(text_font, text.c_str(), text_color);
        auto a = SDL_CreateTextureFromSurface(renderer, text_s);
        SDL_FreeSurface(text_s);
        return a;
    }(renderer);

    int total_height = text_height + title_height + 2;
    int total_width = std::max(text_width , title_width) + 2;

    int x = (w / 2) - (total_width / 2);
    int y = (h / 2) - (total_height / 2);
    SDL_Rect fill_area{x, y, total_width, total_height};

    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(renderer, &fill_area);

    SDL_Rect title_area{x + 1, y + 1, title_width, title_height};
    SDL_Rect text_area{x + 1, y + 2 + title_height, text_width, text_height};

    SDL_RenderCopy(renderer, title_tx, nullptr, &title_area);
    SDL_RenderCopy(renderer, text_tx, nullptr, &text_area);

    SDL_DestroyTexture(text_tx);
    SDL_DestroyTexture(title_tx);
}

bool LineSelector::handle_update(const SDL_Event &evt)
{
    switch (evt.type)
    {
    case SDL_KEYDOWN:
    {
        keybinds_on_down(evt.key.keysym.sym);

        switch (evt.key.keysym.sym)
        {
        case SDLK_BACKSPACE:
        {
            if (text.size() > 0)
            {
                text.pop_back();
            }
        }
        break;
        case SDLK_RETURN:
        {
            this->emplace(this->text);
            is_running = false;
        }
        break;
        case SDLK_ESCAPE:
        {
            this->emplace("");
            is_running = false; // Exit on Escape
        }
        }
        return true; // No further propegation
    }
    break;
    case SDL_KEYUP:
    {
        keybinds_on_up(evt.key.keysym.sym);
        return true; // No further propegation
    }
    break;
    case SDL_TEXTINPUT:
    {
        text.append(evt.text.text);
        return true; // No further propegation
    }
    break;
    }
    return false;
}

bool LineSelector::running()
{
    return is_running;
}

std::shared_ptr<AppLayer> LineSelector::next()
{
    return nullptr;
}
