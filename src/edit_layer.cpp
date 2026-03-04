#include "edit_layer.hpp"

EditLayer::EditLayer(const char *font_path, int font_sz) : font(TTF_OpenFont(font_path, font_sz)),
                                                           status_height(generate_line_height(this->font) + 2),
                                                           line_height(generate_line_height(this->font))
{
    if (!font)
    {
        LOG_FATAL("TTF_OpenFont-Error: %s", TTF_GetError());
        throw std::runtime_error("TTF_OpenFont");
    }
}

int EditLayer::generate_line_height(TTF_Font *font)
{
    const char *testLine = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()";
    int h;
    int w;
    TTF_SizeText(font, testLine, &w, &h);
    return h;
}

EditLayer::~EditLayer()
{
    TTF_CloseFont(font);
}

void EditLayer::render(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    char name[1024] = "<untitled>";

    bool file_saved = false;
    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderClear(renderer);

    render_filename(renderer, w, h, name, file_saved);

    for (size_t i = 0; i < context.lines.size(); ++i)
    {
        render_text_line(renderer, w, h, 0, status_height + (i * line_height), context.lines[i].buffer.c_str());
    }

    render_cursor(renderer, w, h);
}

bool EditLayer::handle_update(const SDL_Event &evt)
{
    switch (evt.type)
    {
    case SDL_QUIT:
    {
        return true;
    }
    break;
    case SDL_KEYDOWN:
    {
        keybinds_on_down(evt.key.keysym.sym);

        switch (evt.key.keysym.sym)
        {
        case SDLK_BACKSPACE:
        {

            context.del_cursor();
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
        }
        break;
        case SDLK_RETURN:
        {

            context.push_line();
        }
        break;
        case SDLK_s:
        {
            // do not check for file*, buffer_write will create the class anyways.
            if (keybinds_is_down(SDLK_LCTRL))
            {
                if (filename)
                {
                    auto fptr = fopen(filename, "w+");
                    if (fptr)
                    {
                        context.write(fptr, filename);
                        fclose(fptr);
                        LOG_INFO("%s saved!", filename);
                    }
                }
                else
                {
                    filename = "Choose a filename!";
                }
            }
        }
        break;
        case SDLK_f:
        {
            // Filemanager keybind: Ctrl + Shift + f
            if (keybinds_is_down(SDLK_LCTRL) && keybinds_is_down(SDLK_LSHIFT))
            {
                // TODO: Open File Manager
            }
        }
        break;
        case SDLK_a:
        {
            // File creation keybind: Ctrl + a (in Filemanager)
            /*
            if (keybinds_is_down(SDLK_LCTRL) && file_man_opened && !filename)
            {
                //TODO: Open File Manager
            }*/
        }
        break;
        }
    }
    break;
    case SDL_KEYUP:
    {
        keybinds_on_up(evt.key.keysym.sym);
    }
    break;
    case SDL_TEXTINPUT:
    {
        context.ins_cursor(evt.text.text);
    }
    break;
    }

    return false;
}

bool EditLayer::running()
{
    return true;
}

std::unique_ptr<AppLayer> EditLayer::next()
{
    return std::unique_ptr<AppLayer>();
}

void EditLayer::render_filename(SDL_Renderer *renderer, int w, int h, const char *name, bool file_saved)
{
    SDL_Rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = w;
    rect.h = status_height;
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderFillRect(renderer, &rect);

    // if (filename)
    // {
    //     strcpy(name, filename);
    // }

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