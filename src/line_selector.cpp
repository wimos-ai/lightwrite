#include "line_selector.hpp"
#include "keybinds.h"
#include "utilities.hpp"
#include "utilities.hpp"

LineSelector::LineSelector(const char *title_font, int title_sz, const char *text_font, int text_sz, const char *title) : title_font(TTF_OpenFont(title_font, title_sz)),
                                                                                                                          text_font(TTF_OpenFont(text_font, text_sz)),
                                                                                                                          title_height(line_height(this->title_font)),
                                                                                                                          text_height(line_height(this->text_font)),
                                                                                                                          title(title)
{
}

LineSelector::~LineSelector()
{
    TTF_CloseFont(title_font);
    TTF_CloseFont(text_font);
}

void LineSelector::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    RasterizedTextInfo title_tx(title_font, renderer, this->title.c_str(), title_color);
    RasterizedTextInfo text_tx(text_font, renderer, this->text.c_str(), text_color);

    int total_height = text_height + title_height + 2;
    int total_width = std::max(title_tx.w, text_tx.w) + 2;

    int x = (w / 2) - (total_width / 2);
    int y = (h / 2) - (total_height / 2);
    SDL_Rect fill_area{x, y, total_width, total_height};

    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderFillRect(renderer, &fill_area);

    title_tx.render(renderer, x + 1, y + 1);
    text_tx.render(renderer, x + 1, y + 2 + title_tx.h);
}

bool LineSelector::handle_update(const SDL_Event &evt)
{
    switch (evt.type)
    {
    case SDL_KEYDOWN:
    {

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
