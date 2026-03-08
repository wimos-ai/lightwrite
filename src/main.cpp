#include "buffer.hpp"
#include "keybinds.hpp"
#include "logger.hpp"
#include "app.hpp"
#include "edit_layer.hpp"
#include "line_selector.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#define FONT_PATH DATA_DIR "/MonoLisaRegular.ttf"
#include <iostream>
#include <file_manager_layer.hpp>
int main(int argc, const char *const *argv)
{
    if (!AppContainer::subsystem_init())
    {
        return EXIT_FAILURE;
    }

    {
        auto ptr = argc == 2 ? std::make_shared<EditLayer>(FONT_PATH, 16, argv[1]) : std::make_shared<EditLayer>(FONT_PATH, 16);
        AppContainer ctnr(ptr, "lightbrite", 800, 600);
        ctnr.run();
    }

    AppContainer::subsystem_destroy();
}