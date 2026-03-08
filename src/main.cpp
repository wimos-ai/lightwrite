#include "buffer.hpp"
#include "keybinds.hpp"
#include "logger.h"
#include "app.hpp"
#include "edit_layer.hpp"
#include "line_selector.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // auto ptr = std::make_shared<FileManagerLayer>(std::filesystem::current_path(),FONT_PATH, 24, FONT_PATH, 16);
        AppContainer ctnr(ptr, "lightbrite", 800, 600);
        ctnr.run();
        // if (ptr->has_value())
        // {
        //     std::cout << ptr->value() << '\n';
        // }
        
    }

    AppContainer::subsystem_destroy();
}