#include "tred/game.h"

#include <map>
#include <string_view>
#include <string>
#include <array>
#include <set>
#include <cassert>

#include "SDL.h"
#include "glad/glad.h"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "tred/log.h"
#include "tred/opengl.debug.h"
#include "tred/types.h"


Game::Game()
    : quad_description
    (
        {
            {VertexType::Position2, "position"},
            {VertexType::Color4, "color"}
        }
    )
    , quad_layout
    (
        Compile({quad_description}).Compile(quad_description)
    )
    , quad_shader
    (
        R"glsl(
            #version 450 core
            in vec3 position;
            in vec4 color;

            uniform mat4 view_projection;
            uniform mat4 transform;

            out vec4 varying_color;

            void main()
            {
                varying_color = color;
                gl_Position = view_projection * transform * vec4(position, 1.0);
            }
        )glsl",
        R"glsl(
            #version 450 core

            in vec4 varying_color;

            layout(location=0) out vec4 color;

            void main()
            {
                color = varying_color;
            }
        )glsl",
        quad_layout
    )
    , view_projection_uniform(quad_shader.GetUniform("view_projection"))
    , transform_uniform(quad_shader.GetUniform("transform"))
{
}

Game::~Game()
{
    quad_shader.Delete();
}


void Game::OnRender(const glm::ivec2&) {}
void Game::OnImgui() {}
bool Game::OnUpdate(float) { return true; }
void Game::OnKey(char, bool) {}
void Game::OnMouseMotion(const glm::ivec2&) {}
void Game::OnMouseButton(int, bool) {}
void Game::OnMouseWheel(int) {}


void SetupOpenGl(SDL_Window* window, SDL_GLContext glcontext, bool imgui)
{
    SetupOpenglDebug();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // remove back faces

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


struct Window
{
    std::string title;
    glm::ivec2 size;
    bool imgui;
    bool running = true;

    SDL_Window* window;
    SDL_GLContext glcontext;

    std::shared_ptr<Game> game;

    Window(const std::string& t, const glm::ivec2& s, bool i)
        : title(t)
        , size(s)
        , imgui(i)
        , window(nullptr)
        , glcontext(nullptr)
    {
        window = SDL_CreateWindow
        (
            t.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            s.x,
            s.y,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if(window == nullptr)
        {
            LOG_ERROR("Could not create window: {}", SDL_GetError());
            return;
        }

        glcontext = SDL_GL_CreateContext(window);

        if(glcontext == nullptr)
        {
            LOG_ERROR("Could not create window: {}", SDL_GetError());

            SDL_DestroyWindow(window);
            window = nullptr;

            return;
        }

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
        {
            LOG_ERROR("Failed to initialize OpenGL context");

            SDL_GL_DeleteContext(glcontext);
            glcontext = nullptr;

            SDL_DestroyWindow(window);
            window = nullptr;

            return;
        }

        SetupOpenGl(window, glcontext, imgui);
    }

    ~Window()
    {
        if(window)
        {
            if(imgui)
            {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplSDL2_Shutdown();
            }

            SDL_GL_DeleteContext(glcontext);
            SDL_DestroyWindow(window);
        }
    }

    void Render()
    {
        if(window == nullptr) { return; }

        game->OnRender(size);

        if(imgui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            game->OnImgui();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(window);
    }

    void OnResized(const glm::ivec2& new_size)
    {
        size = new_size;
    }

    bool HasImgui() const
    {
        return imgui;
    }
};


void PumpEvents(Window* window)
{
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
        bool handle_keyboard = true;
        bool handle_mouse = true;
        if(window->HasImgui())
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
                    window->game->OnKey(static_cast<char>(key), e.type == SDL_KEYDOWN);
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if(handle_mouse)
            {
                window->game->OnMouseMotion(glm::ivec2{e.motion.x, e.motion.y});
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if(handle_mouse)
            {
                window->game->OnMouseButton(e.button.button, e.type == SDL_MOUSEBUTTONDOWN);
            }
            break;

        case SDL_MOUSEWHEEL:
            if(handle_mouse)
            {
                if(e.wheel.y != 0)
                {
                    const auto direction = e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
                    window->game->OnMouseWheel(e.wheel.y * direction);
                }
            }
            break;
        case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                window->OnResized({e.window.data1, e.window.data2});
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


int SetupAndRun(std::function<std::shared_ptr<Game>()> make_game, const std::string& title, const glm::ivec2& size, bool call_imgui)
{
    auto window = Window{title, size, call_imgui};
    if(window.window == nullptr)
    {
        return -1;
    }
    window.game = make_game();

    auto last = SDL_GetPerformanceCounter();

    while(window.running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        PumpEvents(&window);
        if(false == window.game->OnUpdate(dt))
        {
            return 0;
        }
        window.Render();
    }

    return 0;
}


int Run(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game)
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

    const auto ret = SetupAndRun(make_game, title, size, call_imgui);

    SDL_Quit();
    return ret;
}

