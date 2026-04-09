#pragma once

#include "app.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

namespace pfd
{
    class save_file;
}

class FileManagerLayer : public ResourceSelectionLayer<std::filesystem::path>
{
private:
public:
    FileManagerLayer(const std::filesystem::path &dir);

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
    std::unique_ptr<pfd::save_file> file_save_as;
};