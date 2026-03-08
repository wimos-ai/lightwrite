#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <memory>
#include <optional>

class AppLayer
{
public:
    // Render to screen
    virtual void render(SDL_Window *window, SDL_Renderer *renderer, int w, int h) = 0;

    // Handle Event. Return true to stop updates from propegating
    virtual bool handle_update(const SDL_Event &evt) = 0;

    // Return false to end this section of the program
    virtual bool running() = 0;

    virtual std::shared_ptr<AppLayer> next() = 0;

    virtual ~AppLayer() = default;
};

template <typename T>
class ResourceSelectionLayer : public AppLayer, public std::optional<T>
{
};

class AppContainer
{
public:
    static bool subsystem_init();
    static void subsystem_destroy();

public:
    AppContainer(std::shared_ptr<AppLayer> start_layer, const char *name, int w, int h);
    ~AppContainer();

    void run();

private:
    void update_layers();

    void gc_layers();

    void render_layers();

    void check_new_layer();

private:
    void prehandle_evt(const SDL_Event& evt);

    void handle_uncaught_event(const SDL_Event &evt);

private:
    std::vector<std::shared_ptr<AppLayer>> layers;
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};

    int w{0};
    int h{0};

    bool running{true};
};