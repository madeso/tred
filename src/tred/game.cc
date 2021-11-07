#include "tred/game.h"

#include <map>
#include <string_view>
#include <string>
#include <array>
#include <set>
#include <cassert>

#include "glad/glad.h"

// imgui
#include "tred/dependency_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "tred/dependency_sdl.h"
#include "tred/dependency_glm.h"
#include "tred/log.h"
#include "tred/opengl_debug.h"
#include "tred/opengl_utils.h"
#include "tred/types.h"
#include "tred/image.h"
#include "tred/viewportdef.h"
#include "tred/windows.sdl.convert.h"


void Game::on_render(const RenderCommand2&) {}
void Game::on_imgui() {}
bool Game::on_update(float) { return true; }
void Game::on_key(char, bool) {}
void Game::on_mouse_position(const Command2&, const glm::ivec2&) {}
void Game::on_mouse_button(const Command2&, input::MouseButton, bool) {}
void Game::on_mouse_wheel(int) {}

namespace
{
    void setup_open_gl(SDL_Window* window, SDL_GLContext glcontext, bool imgui)
    {
        opengl_setup();
        opengl_set2d();

        const auto* renderer = glGetString(GL_RENDERER); // get renderer string
        const auto* version = glGetString(GL_VERSION); // version as a string
        LOG_INFO("Renderer: {}", renderer);
        LOG_INFO("Version: {}", version);

        if(!imgui) { return; }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsLight();

        ImGui_ImplSDL2_InitForOpenGL(window, glcontext);

        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }
}


struct Window
{
    std::string title;
    glm::ivec2 size;
    bool imgui;
    bool running = true;

    SDL_Window* sdl_window;
    SDL_GLContext sdl_glcontext;

    std::shared_ptr<Game> game;
    std::unique_ptr<Render2> render_data;

    Window(const std::string& t, const glm::ivec2& s, bool i)
        : title(t)
        , size(s)
        , imgui(i)
        , sdl_window(nullptr)
        , sdl_glcontext(nullptr)
    {
        sdl_window = SDL_CreateWindow
        (
            t.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            s.x,
            s.y,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if(sdl_window == nullptr)
        {
            LOG_ERROR("Could not create window: {}", SDL_GetError());
            return;
        }

        sdl_glcontext = SDL_GL_CreateContext(sdl_window);

        if(sdl_glcontext == nullptr)
        {
            LOG_ERROR("Could not create window: {}", SDL_GetError());

            SDL_DestroyWindow(sdl_window);
            sdl_window = nullptr;

            return;
        }

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
        {
            LOG_ERROR("Failed to initialize OpenGL context");

            SDL_GL_DeleteContext(sdl_glcontext);
            sdl_glcontext = nullptr;

            SDL_DestroyWindow(sdl_window);
            sdl_window = nullptr;

            return;
        }

        setup_open_gl(sdl_window, sdl_glcontext, imgui);
    }

    ~Window()
    {
        if(sdl_window)
        {
            if(imgui)
            {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplSDL2_Shutdown();
            }

            SDL_GL_DeleteContext(sdl_glcontext);
            SDL_DestroyWindow(sdl_window);
        }
    }

    void render() const
    {
        if(sdl_window == nullptr) { return; }

        glViewport(0, 0, size.x, size.y);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        game->on_render({render_data.get(), size});

        if(imgui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(sdl_window);
            ImGui::NewFrame();

            game->on_imgui();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(sdl_window);
    }

    void on_resized(const glm::ivec2& new_size)
    {
        size = new_size;
    }

    bool has_imgui() const
    {
        return imgui;
    }
};


void pump_events(Window* window)
{
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
        bool handle_keyboard = true;
        bool handle_mouse = true;
        if(window->has_imgui())
        {
            ImGui_ImplSDL2_ProcessEvent(&e);

            auto& io = ImGui::GetIO();
            handle_mouse = !io.WantCaptureMouse;
            handle_keyboard = !io.WantCaptureKeyboard;
        }

        switch(e.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if(handle_keyboard){
                const auto key = e.key.keysym.sym;
                if(key >= SDLK_BACKSPACE && key <= SDLK_DELETE)
                {
                    window->game->on_key(static_cast<char>(key), e.type == SDL_KEYDOWN);
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if(handle_mouse)
            {
                window->game->on_mouse_position({window->size}, glm::ivec2{e.motion.x, window->size.y - e.motion.y});
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if(handle_mouse)
            {
                window->game->on_mouse_button({window->size}, sdl::to_mouse_button(e.button.button), e.type == SDL_MOUSEBUTTONDOWN);
            }
            break;

        case SDL_MOUSEWHEEL:
            if(handle_mouse)
            {
                if(e.wheel.y != 0)
                {
                    const auto direction = e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
                    window->game->on_mouse_wheel(e.wheel.y * direction);
                }
            }
            break;
        case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                window->on_resized({e.window.data1, e.window.data2});
            }
            break;
        case SDL_QUIT:
            window->running = false;
            break;
        break;
        default:
            // ignore other events
            break;
        }
    }
}


int setup_and_run(std::function<std::shared_ptr<Game>()> make_game, const std::string& title, const glm::ivec2& size, bool call_imgui)
{
    auto window = ::Window{title, size, call_imgui};
    if(window.sdl_window == nullptr)
    {
        return -1;
    }
    window.render_data = std::make_unique<Render2>();
    window.game = make_game();

    auto last = SDL_GetPerformanceCounter();

    while(window.running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        pump_events(&window);
        if(false == window.game->on_update(dt))
        {
            return 0;
        }
        window.render();
    }

    return 0;
}


int run_game(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game)
{
    constexpr Uint32 flags =
          SDL_INIT_VIDEO
        ;

    if(SDL_Init(flags) != 0)
    {
        LOG_ERROR("Unable to initialize SDL: {}", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const auto ret = setup_and_run(make_game, title, size, call_imgui);

    SDL_Quit();
    return ret;
}

