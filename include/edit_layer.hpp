#pragma once
#include <optional>

#include "app.hpp"
#include "buffer.hpp"
#include "font.h"
#include "keybinds.h"
#include "logger.h"
#include "filemanager.hpp"
#include "app.hpp"

class EditLayer : public AppLayer
{
public:
    EditLayer(const char *font_path, int font_sz);
    EditLayer(const char *font_path, int font_sz, const char* fname);
    ~EditLayer();

public:
    // Render to screen
    void render(SDL_Window *window, SDL_Renderer *renderer, int w, int h) override;

    // Handle Event. Return true to stop updates from propegating
    bool handle_update(const SDL_Event &evt) override;

    // Return false to end this section of the program
    bool running() override;

    std::shared_ptr<AppLayer> next() override;

private:
    void render_filename(SDL_Renderer *renderer, int w, int h, bool file_saved);
    void render_text_line(SDL_Renderer *renderer, int w, int h, int lx, int ly, const char *line);
    void render_cursor(SDL_Renderer *renderer, int w, int h);

private:
    static int generate_line_height(TTF_Font *font);

private:
    TTF_Font *font;
    Buffer context;
    std::optional<std::string> filename{std::nullopt};
    SDL_Color text_color = {255, 255, 255, 255};
    const int status_height;
    const int line_height;
    bool file_saved;
};