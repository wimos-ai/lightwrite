#pragma once
#include "app.hpp"

#include "font.h"

class LineSelector : public ResourceSelectionLayer<std::string>
{
public:
    LineSelector(const char *title_font, int title_sz, const char *text_font, int text_sz, const char *title);
    ~LineSelector();

public:
    // Render to screen
    void render(SDL_Window *window, SDL_Renderer *renderer, int w, int h) override;

    // Handle Event. Return true to stop updates from propegating
    virtual bool handle_update(const SDL_Event &evt) override;

    // Return false to end this section of the program
    virtual bool running() override;

    virtual std::shared_ptr<AppLayer> next() override;

private:
    static int generate_line_height(TTF_Font *font);

private:
    TTF_Font *title_font{nullptr};
    TTF_Font *text_font{nullptr};
    int title_height;
    int text_height;
    std::string title;
    std::string text;
    bool is_running{true};

    const SDL_Color title_color{255, 255, 255, 255};
    const SDL_Color text_color{255, 255, 255, 255};
    const SDL_Color bg_color{14, 14, 14, 255};
};