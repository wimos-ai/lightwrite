#pragma once
#include <optional>
#include <filesystem>

#include "app.hpp"
#include "buffer.hpp"
#include "keybinds.hpp"
#include "logger.hpp"
#include "app.hpp"
#include "line_selector.hpp"
#include "file_manager_layer.hpp"
#include "utilities.hpp"

class EditLayer : public AppLayer
{
public:
    EditLayer(const char *font_path, int font_sz);
    EditLayer(const char *font_path, int font_sz, const char *fname);
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
    void render_cursor(SDL_Renderer *renderer, int w, int h, int y, std::string_view sv, std::string_view::iterator cursor);
    void render_active_line(const Buffer::Line &ln, SDL_Renderer *renderer, int w, int h, int x, int y);

    void save_buffer();

private:
    void init_fonts();
    void inc_scaling();
    void dec_scaling();

private:
    const char *font_path;
    int base_ft_size;
    float ft_scale = 1.0;

private:
    TTF_Font *font;
    int status_height;
    int line_height;

private:
    Buffer context;
    std::optional<std::filesystem::path> filename{std::nullopt};
    SDL_Color text_color = {255, 255, 255, 255};
    bool file_saved;
    std::optional<std::shared_ptr<AppLayer>> next_layer;
    std::optional<std::shared_ptr<FileManagerLayer>> fman;
    std::optional<std::shared_ptr<LineSelector>> select_save_line;
};