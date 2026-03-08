#include "keybinds.hpp"
#include "logger.hpp"

#include <SDL2/SDL_keycode.h>
#include <array>

struct Keybind
{
    int keycode;
    bool pressed;
};

static std::array keybinds{
    Keybind{SDLK_LCTRL, false},
    Keybind{SDLK_LSHIFT, false}};

void keybinds_on_up(int keycode)
{
    for (auto &keybind : keybinds)
    {
        if (keybind.keycode == keycode)
        {
            keybind.pressed = false;
        }
    }
}

void keybinds_on_down(int keycode)
{
    for (auto &keybind : keybinds)
    {
        if (keybind.keycode == keycode)
        {
            keybind.pressed = true;
        }
    }
}

bool keybinds_is_down(int keycode)
{
    for (auto &keybind : keybinds)
    {
        if (keybind.keycode == keycode)
        {
            return keybind.pressed;
        }
    }
    return false;
}
