#pragma once

#include "app.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include "line_selector.hpp"

class FileManagerLayer : public ResourceSelectionLayer<std::string>
{
public:
    FileManagerLayer(const char *directory, const char *title_font, int title_sz, const char *text_font, int text_sz);

    ~FileManagerLayer();

public:
    // Render to screen
    void render(SDL_Window *window, SDL_Renderer *renderer, int w, int h) override;

    // Handle Event. Return true to stop updates from propegating
    bool handle_update(const SDL_Event &evt) override;

    // Return false to end this section of the program
    bool running() override;

    std::shared_ptr<AppLayer> next() override;

private:
    static std::vector<std::string> files_from_dir(std::filesystem::path& p);

private:
    std::filesystem::path dir;
    TTF_Font *title_font{nullptr};
    TTF_Font *text_font{nullptr};
    std::string title;
    int title_height;
    int text_height;
    std::vector<std::string> files;
    size_t start_render_idx{0};
    bool is_running{true};
    std::optional<std::shared_ptr<LineSelector>> line_sel;

    const SDL_Color title_color{255, 255, 255, 255};
    const SDL_Color text_color{255, 255, 255, 255};
    const SDL_Color bg_color{14, 14, 14, 255};
};