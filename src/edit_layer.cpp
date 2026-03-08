#include "edit_layer.hpp"
#include "utilities.hpp"

#include <iostream>
#include <algorithm>
#define FONT_PATH DATA_DIR "/MonoLisaRegular.ttf"

EditLayer::EditLayer(const char *font_path, int font_sz) : font(TTF_OpenFont(font_path, font_sz)),
                                                           status_height(::line_height(this->font) + 2),
                                                           line_height(::line_height(this->font)),
                                                           file_saved(false)
{
    if (!font)
    {
        LOG_FATAL("TTF_OpenFont-Error: %s", TTF_GetError());
        throw std::runtime_error("TTF_OpenFont");
    }
}

EditLayer::EditLayer(const char *font_path, int font_sz, const char *fname) : EditLayer(font_path, font_sz)
{
    filename = fname;

    this->context = Buffer::read(fname);

    file_saved = true;
}

EditLayer::~EditLayer()
{
    TTF_CloseFont(font);
}

void EditLayer::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{

    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255));
    LOG_SDL_ERROR(SDL_RenderClear(renderer));

    render_filename(renderer, w, h, file_saved);

    size_t nloops{0};
    for (size_t i = context.cursor; i < context.lines.size(); (i++, nloops++))
    {
        if (!context.lines[i].buffer.empty())
        {
            RasterizedTextInfo line(font, renderer, context.lines[i].buffer.c_str(), text_color);
            line.render(renderer, 0, status_height + (nloops * line_height));
        }
    }

    render_cursor(renderer, w, h);
}

bool EditLayer::handle_update(const SDL_Event &evt)
{
    if (select_save_line.has_value() && !select_save_line.value()->running())
    {

        if (select_save_line.value()->has_value())
        {
            filename = select_save_line.value()->value();
            save_buffer();
        }
        select_save_line = std::nullopt;
    }

    if (fman.has_value() && !fman.value()->running())
    {
        if (fman.value()->has_value())
        {
            auto fname = fman.value()->value();

            try
            {
                this->context = Buffer::read(fname.c_str());
            }
            catch (const std::exception &e)
            {
                // File does not exist
                this->context = Buffer();
            }
            this->filename = fname;
        }
        fman = std::nullopt;
    }

    switch (evt.type)
    {
    case SDL_QUIT:
    {
        return true;
    }
    break;
    case SDL_KEYDOWN:
    {
        switch (evt.key.keysym.sym)
        {
        case SDLK_BACKSPACE:
        {
            context.del_cursor();
            file_saved = false;
        }
        break;
        case SDLK_LEFT:
        {
            if (context.get_cursor_row() > 0)
            {
                context.lines[context.cursor].cursor--;
            }
        }
        break;
        case SDLK_RIGHT:
        {
            if (context.get_cursor_row() <
                context.lines[context.cursor].buffer.size())
            {
                context.lines[context.cursor].cursor++;
            }
        }
        break;
        case SDLK_UP:
        {
            if (context.cursor > 0)
            {
                context.cursor--;
                context.lines[context.cursor].cursor =
                    context.lines[context.cursor].buffer.size();
            }
        }
        break;
        case SDLK_DOWN:
        {

            if (context.cursor < (context.lines.size() - 1))
            {
                context.cursor++;
                context.lines[context.cursor].cursor =
                    context.lines[context.cursor].buffer.size();
            }
        }
        break;
        case SDLK_DELETE:
        {
            context.del();
            file_saved = false;
        }
        break;
        case SDLK_RETURN:
        {

            context.push_line();
            file_saved = false;
        }
        break;
        case SDLK_s:
        {
            // do not check for file*, buffer_write will create the class anyways.
            if (keybinds_is_down(SDLK_LCTRL))
            {
                save_buffer();
            }
        }
        break;
        case SDLK_f:
        {
            // Filemanager keybind: Ctrl + Shift + f
            if (keybinds_is_down(SDLK_LCTRL) && keybinds_is_down(SDLK_LSHIFT))
            {
                if (filename.has_value())
                {
                    fman.emplace(std::make_shared<FileManagerLayer>(filename.value().parent_path(), FONT_PATH, 24, FONT_PATH, 16));
                }
                else
                {
                    try
                    {
                        auto cwd = std::filesystem::current_path();
                        fman.emplace(std::make_shared<FileManagerLayer>(cwd, FONT_PATH, 24, FONT_PATH, 16));
                    }
                    catch (std::filesystem::filesystem_error &e)
                    {
                        LOG_ERROR("std::filesystem::filesystem_error: #%d: %s", e.code(), e.what());
                    }
                }

                next_layer = fman;
            }
        }
        break;
        }
    }
    break;
    case SDL_TEXTINPUT:
    {
        context.ins_cursor(evt.text.text);
        file_saved = false;
    }
    break;
    case SDL_MOUSEWHEEL:
    {
        int d_l = -evt.wheel.y;
        int cf = std::clamp((int)context.cursor + d_l, 0, (int)context.lines.size()-1);
        context.cursor = cf;
    }
    break;
    }

    return false;
}

void EditLayer::save_buffer()
{
    if (filename.has_value())
    {
        const char *fname = filename.value().c_str();

        context.write(fname);
        LOG_INFO("%s saved!", fname);
        file_saved = true;
    }
    else
    {
        select_save_line = std::make_shared<LineSelector>("/home/willm/Desktop/lightwrite/data/MonoLisaRegular.ttf", 24, "/home/willm/Desktop/lightwrite/data/MonoLisaRegular.ttf", 16, "Choose File Name:");
        next_layer = select_save_line;
    }
}

bool EditLayer::running()
{
    return true;
}

std::shared_ptr<AppLayer> EditLayer::next()
{
    if (next_layer.has_value())
    {
        return std::move(next_layer.value());
    }
    else
    {
        return {};
    }
}

void EditLayer::render_filename(SDL_Renderer *renderer, int w, int h, bool file_saved)
{
    std::string name = filename.has_value() ? filename.value().c_str() : "<untitled>";
    if (!file_saved)
    {
        name.append("*");
    }

    SDL_Rect rect{0, 0, w, status_height};
    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255));
    LOG_SDL_ERROR(SDL_RenderFillRect(renderer, &rect));

    RasterizedTextInfo inf(this->font, renderer, name.c_str(), this->text_color);
    inf.render(renderer, 0, 0);
}

void EditLayer::render_cursor(SDL_Renderer *renderer, int w, int h)
{

    int width;  // used for font-width
    int height; // used for font-height

    auto str = context.get().buffer.substr(0, context.get().cursor);

    LOG_TTF_ERROR(TTF_SizeUTF8(font, str.c_str(), &width, &height));

    // Render Cursor
    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255));
    SDL_Rect rect;
    rect.x = width;
    rect.y = status_height;
    rect.w = 3;
    rect.h = height;
    LOG_SDL_ERROR(SDL_RenderFillRect(renderer, &rect));
}