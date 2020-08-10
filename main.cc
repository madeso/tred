#include "SDL.h"

int
main(int, char**)
{
    // int width = 1280;
    // int height = 720;

    bool running = false;

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                const bool down = e.type == SDL_KEYDOWN;

                switch(e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    if(down)
                    {
                        running = false;
                    }
                    break;
                default:
                    // ignore other keys
                    break;
                }
            }
            break;
            default:
                // ignore other events
                break;
            }
        }

        // ClearScreen(Color::Black);
        // world.Render(viewport_handler.GetFullViewport(), camera);
        // SDL_GL_SwapWindow(window);
    }

    return 0;
}

