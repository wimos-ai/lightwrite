#include "file_manager_layer.hpp"
#include <filesystem>
#include <iostream>
#include <algorithm>

#include "portable_filedialogs.hpp"

namespace fs = std::filesystem;

FileManagerLayer::FileManagerLayer(const std::filesystem::path &directory_) : file_save_as(std::make_unique<pfd::save_file>("Choose file to save",
                                                                                                                            directory_,
                                                                                                                            std::vector<std::string>{"All Files", "*"},
                                                                                                                            pfd::opt::force_overwrite))
{
}

FileManagerLayer::~FileManagerLayer()
{
}

void FileManagerLayer::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    return;
}

bool FileManagerLayer::handle_update(const SDL_Event &evt)
{
    return true;
}

bool FileManagerLayer::running()
{
    bool is_ready = file_save_as->ready();
    if (is_ready)
    {
        auto result = file_save_as->result();
        if (result.size() > 0)
        {
            this->emplace(result);
        }
        return false;
    }
    else
    {
        return true;
    }
}

std::shared_ptr<AppLayer> FileManagerLayer::next()
{
    return {};
}
