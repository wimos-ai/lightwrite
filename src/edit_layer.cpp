#include "edit_layer.hpp"
#include "utilities.hpp"

#include <iostream>
#include <algorithm>
#include <cstring>
#define FONT_PATH DATA_DIR "/MonoLisaRegular.ttf"

EditLayer::EditLayer(const char *font_path_, int font_sz) : font_path(font_path_),
                                                            base_ft_size(font_sz),
                                                            font(TTF_OpenFont(font_path, base_ft_size * ft_scale)),
                                                            status_height(TTF_FontHeight(this->font) + 2),
                                                            line_height(TTF_FontHeight(this->font)),
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

void EditLayer::init_fonts()
{
    TTF_CloseFont(font);
    font = TTF_OpenFont(font_path, base_ft_size * ft_scale);
    this->status_height = TTF_FontHeight(this->font) + 2;
    this->line_height = TTF_FontHeight(this->font);
}

void EditLayer::inc_scaling()
{
    this->ft_scale += 0.125;
    init_fonts();
}

void EditLayer::dec_scaling()
{
    this->ft_scale -= 0.125;
    init_fonts();
}

void EditLayer::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{

    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255));
    LOG_SDL_ERROR(SDL_RenderClear(renderer));

    render_filename(renderer, w, h, file_saved);

    auto lines = context.get_render_zone((h - this->status_height) / line_height);
    auto sel_line = context.get_active_line();
    size_t nloops{0};
    for (auto it = lines.first; it != lines.second; (it++, nloops++))
    {
        if (it->size() > 0)
        {

            if (it == sel_line)
            {
                this->render_active_line(*it, renderer, w, h, 0, status_height + (nloops * line_height));
            }
            else
            {
                RasterizedTextInfo line(font, renderer, *it, text_color);
                line.render(renderer, 0, status_height + (nloops * line_height));
            }
        }
    }
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
            context.left();
        }
        break;
        case SDLK_RIGHT:
        {
            context.right();
        }
        break;
        case SDLK_UP:
        {
            context.up();
        }
        break;
        case SDLK_DOWN:
        {

            context.down();
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

        case SDLK_EQUALS:
        {
            if (keybinds_is_down(SDLK_LCTRL) && keybinds_is_down(SDLK_LSHIFT))
            {
                inc_scaling();
                return true;
            }
        }
        break;
        case SDLK_MINUS:
        {
            if (keybinds_is_down(SDLK_LCTRL) && keybinds_is_down(SDLK_LSHIFT))
            {
                dec_scaling();
                return true;
            }
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
        case SDLK_HOME:
        {
            this->context.get_active_line()->reset_cursor();
        }
        break;
        case SDLK_END:
        {
            this->context.get_active_line()->end_cursor();
        }
        break;
        }
    }
    break;
    case SDL_TEXTINPUT:
    {
        // Ignore misc charecters when using the ctrl + shift + PLUS and ctrl + shift + - controls
        if (keybinds_is_down(SDLK_LCTRL) && keybinds_is_down(SDLK_LSHIFT))
        {
            if (std::min(std::strlen(evt.text.text), sizeof(evt.text.text)) == 1)
            {
                if (evt.text.text[0] == '_' || evt.text.text[0] == '+')
                {
                    break;
                }
            }
        }
        context.ins_cursor(evt.text.text);
        file_saved = false;
    }
    break;
    case SDL_MOUSEWHEEL:
    {
        context.scroll(-evt.wheel.y);
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

void EditLayer::render_cursor(SDL_Renderer *renderer, int w, int h, int y, std::string_view sv, std::string_view::iterator cursor)
{

    int width;  // used for font-width
    int height; // used for font-height

    auto str = std::string_view(sv.begin(), cursor);

    LOG_TTF_ERROR(TTF_SizeUTF8(font, RasterizedTextInfo::sv_fwd_helper(str), &width, &height));

    // Render Cursor
    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255));
    SDL_Rect rect;
    rect.x = width;
    rect.y = y;
    rect.w = 3;
    rect.h = height;
    LOG_SDL_ERROR(SDL_RenderFillRect(renderer, &rect));
}

void EditLayer::render_active_line(const Buffer::Line &ln, SDL_Renderer *renderer, int w, int h, int x, int y)
{
    int extent{0};
    int count{0};
    LOG_TTF_ERROR(TTF_MeasureUTF8(font,  RasterizedTextInfo::sv_fwd_helper(ln), w, &extent, &count));

    auto [substr, new_cursor] = ln.get_render_window(count);

    RasterizedTextInfo line(font, renderer, substr, text_color);
    line.render(renderer, x, y);
    render_cursor(renderer, w, h, y, substr, substr.begin() + std::distance(substr.begin(), new_cursor));
}
