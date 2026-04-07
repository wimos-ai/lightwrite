#include "file_manager_layer.hpp"
#include <filesystem>
#include <iostream>
#include <algorithm>

#include "utilities.hpp"
#include "keybinds.hpp"
#include "logger.hpp"

namespace fs = std::filesystem;

FileManagerLayer::FileManagerLayer(const std::filesystem::path &directory_, const char *title_font, int title_sz, const char *text_font, int text_sz) : dir(directory_),
                                                                                                                                                        title_font(TTF_OpenFont(title_font, title_sz)),
                                                                                                                                                        text_font(TTF_OpenFont(text_font, text_sz)),
                                                                                                                                                        title(std::string("Directory: ") + dir.c_str()),
                                                                                                                                                        title_height(TTF_FontHeight(this->title_font)),
                                                                                                                                                        files(files_from_dir(dir)),
                                                                                                                                                        text_height(TTF_FontHeight(this->text_font))
{
    if (!title_font)
    {
        LOG_FATAL("TTF_OpenFont-Error: %s", TTF_GetError());
        throw std::runtime_error("TTF_OpenFont");
    }

    if (!text_font)
    {
        LOG_FATAL("TTF_OpenFont-Error: %s", TTF_GetError());
        throw std::runtime_error("TTF_OpenFont");
    }
}

FileManagerLayer::~FileManagerLayer()
{
    TTF_CloseFont(title_font);
    TTF_CloseFont(text_font);
}

void FileManagerLayer::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a));
    SDL_Rect r{0, 0, w, h};
    LOG_SDL_ERROR(SDL_RenderFillRect(renderer, &r));

    RasterizedTextInfo title(title_font, renderer, this->title.c_str(), title_color);

    title.render(renderer, 0, 0);

    size_t nloop{0};
    size_t max_loop = (h - title.h) / this->text_height;
    for (size_t i = this->start_render_idx; i < this->files.size() && nloop < max_loop; (i++, nloop++))
    {
        int line_y = title_height + (text_height * nloop);
        if (i == this->start_render_idx)
        {
            RasterizedTextInfo line(text_font, renderer, this->files[i].c_str(), get_color_negitive(text_color));
            SDL_Rect r{0, line_y, line.w, line.h};
            auto sel_bg{get_color_negitive(bg_color)};
            LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, sel_bg.r, sel_bg.g, sel_bg.b, sel_bg.a));
            LOG_SDL_ERROR(SDL_RenderFillRect(renderer, &r));
            line.render(renderer, 0, title_height + (text_height * nloop));
        }
        else
        {
            RasterizedTextInfo line(text_font, renderer, this->files[i].c_str(), text_color);
            line.render(renderer, 0, title_height + (text_height * nloop));
        }
    }
}

bool FileManagerLayer::handle_update(const SDL_Event &evt)
{
    if (line_sel.has_value() && !line_sel.value()->running())
    {
        if (line_sel.value()->has_value())
        {
            auto str = line_sel.value()->value();
            if (str.size() > 0)
            {
                this->emplace(dir / str);
                this->is_running = false;
            }
        }
        line_sel = std::nullopt;
    }

    if (evt.type == SDL_KEYDOWN)
    {
        if (evt.key.keysym.sym == SDLK_UP)
        {
            if (this->start_render_idx != 0)
            {
                this->start_render_idx--;
            }
        }
        if (evt.key.keysym.sym == SDLK_DOWN)
        {
            if (this->start_render_idx != files.size() - 1)
            {
                this->start_render_idx++;
            }
        }
        if (evt.key.keysym.sym == SDLK_RETURN)
        {
            if (this->files.size() > 0)
            {
                this->emplace(dir / files[this->start_render_idx]);
                is_running = false;
            }
        }
        if (evt.key.keysym.sym == SDLK_ESCAPE)
        {
            is_running = false;
        }
        if (evt.key.keysym.sym == SDLK_a)
        {
            if (keybinds_is_down(SDLK_LCTRL))
            {
                this->line_sel = std::make_shared<LineSelector>("/home/willm/Desktop/lightwrite/data/MonoLisaRegular.ttf", 24, "/home/willm/Desktop/lightwrite/data/MonoLisaRegular.ttf", 16, "Choose New File Name:");
            }
        }
    }
    return false;
}

bool FileManagerLayer::running()
{
    return is_running;
}

std::shared_ptr<AppLayer> FileManagerLayer::next()
{
    if (line_sel.has_value())
    {
        return line_sel.value();
    }
    else
    {
        return nullptr;
    }
}

std::vector<std::string> FileManagerLayer::files_from_dir(std::filesystem::path &p)
{
    std::vector<std::string> files;

    for (const auto &entry : fs::directory_iterator(p))
    {
        if (entry.is_regular_file())
        {
            fs::path relative = fs::relative(entry.path(), p);
            files.emplace_back(relative);
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}
