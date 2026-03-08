#include "app.hpp"

#include "logger.hpp"
#include "keybinds.hpp"

bool AppContainer::subsystem_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LOG_FATAL("SDL-Error: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0)
    {
        LOG_FATAL("TTF-Error: %s", TTF_GetError());
        SDL_Quit();
        return false;
    }
    return true;
}

void AppContainer::subsystem_destroy()
{
    TTF_Quit();
    SDL_Quit();
}

AppContainer::AppContainer(std::shared_ptr<AppLayer> start_layer, const char *name, int w_, int h_) : w(w_),
                                                                                                      h(h_)
{
    layers.emplace_back(std::move(start_layer));
    window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h,
                              SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        LOG_FATAL("SDL-Error: %s", SDL_GetError());
        throw std::runtime_error("Window Create");
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        LOG_FATAL("SDL-Error: %s", SDL_GetError());
        throw std::runtime_error("Renderer Create");
    }
}

AppContainer::~AppContainer()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void AppContainer::update_layers()
{
    // Update layers in top down
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        prehandle_evt(evt);
        for (auto itr = layers.rbegin(); itr != layers.rend(); itr++)
        {
            // If the event is handled by the layer, stop propegating it
            if ((*itr)->handle_update(evt))
            {
                break;
            }
        }
        handle_uncaught_event(evt);
    }
}

void AppContainer::render_layers()
{
    // Render from bottom up
    LOG_SDL_ERROR(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
    LOG_SDL_ERROR(SDL_RenderClear(renderer));
    for (auto &layer : layers)
    {
        layer->render(window, renderer, w, h);
    }
    SDL_RenderPresent(renderer);
}

void AppContainer::check_new_layer()
{
    auto next = layers.back()->next();
    if (next)
    {
        layers.emplace_back(std::move(next));
    }
}

void AppContainer::prehandle_evt(const SDL_Event &evt)
{
    if (evt.type == SDL_WINDOWEVENT)
    {
        if (evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && renderer)
        {
            w = evt.window.data1;
            h = evt.window.data2;
            LOG_SDL_ERROR(SDL_RenderSetLogicalSize(renderer, w, h));
        }
    }
    if (evt.type == SDL_KEYDOWN)
    {
        keybinds_on_down(evt.key.keysym.sym);
    }
    if (evt.type == SDL_KEYUP)
    {
        keybinds_on_up(evt.key.keysym.sym);
    }
}

void AppContainer::handle_uncaught_event(const SDL_Event &evt)
{
    if (evt.type == SDL_QUIT)
    {
        this->running = false;
    }
}

void AppContainer::gc_layers()
{
    // Reversed gc order so children die before parents
    for (size_t i = layers.size(); i-- > 0;)
    {
        if (!layers[i]->running())
        {
            layers.erase(layers.begin() + i);
        }
    }
}

void AppContainer::run()
{
    while (layers.size() > 0 && this->running)
    {
        update_layers();

        check_new_layer();

        gc_layers();

        render_layers();
    }
}