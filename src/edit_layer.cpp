#include "edit_layer.hpp"
#include "utilities.hpp"
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

    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderClear(renderer);

    render_filename(renderer, w, h, file_saved);

    for (size_t i = 0; i < context.lines.size(); ++i)
    {
        render_text_line(renderer, w, h, 0, status_height + (i * line_height), context.lines[i].buffer.c_str());
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

            std::filesystem::path f{fname};

            if (f.has_filename()) // This is a file
            {
                try
                {
                    this->context = Buffer::read(f.c_str());
                }
                catch(const std::exception& e)
                {
                    // File does not exist
                    this->context = Buffer();
                }
                this->filename = f;
                
            }
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
                fman.emplace(std::make_shared<FileManagerLayer>(filename.value_or(".").c_str(), FONT_PATH, 24, FONT_PATH, 16));
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
    const char *name;
    if (filename.has_value())
    {
        name = filename.value().c_str();
    }
    else
    {
        name = "<untitled>";
    }

    SDL_Rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = w;
    rect.h = status_height;
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderFillRect(renderer, &rect);

    Font_Data data = prepare_string(font, renderer, 1, 1, name, text_color);
    SDL_RenderCopy(renderer, data.texture, NULL, &data.rect);
    SDL_DestroyTexture(data.texture);

    if (!file_saved)
    {
        int width, height;
        TTF_SizeText(font, name, &width, &height);

        SDL_RenderSetScale(renderer, 0.9f, 0.9f);
        data = prepare_string(font, renderer, (2 + width) * 1.1f, 2, "*", text_color);
        SDL_RenderCopy(renderer, data.texture, NULL, &data.rect);
        SDL_DestroyTexture(data.texture);
        SDL_RenderSetScale(renderer, 1.f, 1.f);
    }
}

void EditLayer::render_text_line(SDL_Renderer *renderer, int w, int h, int lx, int ly, const char *line)
{
    int width;  // used for font-width
    int height; // used for font-height
    TTF_SizeText(font, line, &width, &height);

    Font_Data data = prepare_string(font, renderer, lx, ly, line, text_color);
    SDL_RenderCopy(renderer, data.texture, NULL, &data.rect);
    SDL_DestroyTexture(data.texture);
}

void EditLayer::render_cursor(SDL_Renderer *renderer, int w, int h)
{
    char substr[4096];

    int width;  // used for font-width
    int height; // used for font-height
    // Getting the string written from the left of the screen until the cursor.
    memset(substr, 0, sizeof(substr));
    strncpy(substr, context.lines[context.cursor].buffer.c_str(), context.lines[context.cursor].cursor);

    TTF_SizeText(font, substr, &width, &height);

    // Render Cursor
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect;
    rect.x = width;
    rect.y = status_height + (context.cursor * height);
    rect.w = 3;
    rect.h = height;
    SDL_RenderFillRect(renderer, &rect);
}