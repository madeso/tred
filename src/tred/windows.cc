#include "tred/windows.h"

#include <map>

#include "SDL.h"
#include "glad/glad.h"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "tred/log.h"
#include "tred/opengl.debug.h"

#include "tred/input/system.h"
#include "tred/input/sdl-platform.h"

using WindowId = Uint32;


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


struct WindowImpl : public detail::Window
{
    std::string title;
    glm::ivec2 size;
    RenderFunction on_render;
    std::optional<ImguiFunction> imgui;

    SDL_Window* window;
    SDL_GLContext glcontext;

    WindowImpl(const std::string& t, const glm::ivec2& s, RenderFunction&& r, std::optional<ImguiFunction>&& i)
        : title(t)
        , size(s)
        , on_render(r)
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

        SetupOpenGl(window, glcontext, imgui.has_value());
    }

    ~WindowImpl()
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

    void Render() override
    {
        if(window == nullptr) { return; }

        on_render(size);

        if(imgui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            (*imgui)();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(window);
    }

    void OnResized(const glm::ivec2& new_size)
    {
        size = new_size;
    }

    WindowId GetId() const
    {
        if(window == nullptr) { return 0; }
        return SDL_GetWindowID(window);
    }

    bool HasImgui() const
    {
        return imgui.has_value();
    }
};


void Windows::AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render)
{
    AddWindowImpl(title, size, std::move(on_render), std::nullopt);
}


void Windows::AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, ImguiFunction&& on_imgui)
{
    AddWindowImpl(title, size, std::move(on_render), std::move(on_imgui));
}


struct WindowsImpl : public Windows
{
    std::map<WindowId, std::unique_ptr<WindowImpl>> windows;
    input::SdlPlatform platform;

    explicit WindowsImpl()
    {
    }

    ~WindowsImpl()
    {
        SDL_Quit();
    }

    void AddWindowImpl(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, std::optional<ImguiFunction>&& imgui) override
    {
        auto window = std::make_unique<WindowImpl>(title, size, std::move(on_render), std::move(imgui));
        windows[window->GetId()] = std::move(window);
    }

    void Render() override
    {
        for(auto& window: windows)
        {
            window.second->Render();
        }
    }

    bool HasImgui()
    {
        for(auto& window: windows)
        {
            if(window.second->HasImgui())
            {
                return true;
            }
        }

        return false;
    }

    input::Platform* GetInputPlatform() override
    {
        return &platform;
    }

    void PumpEvents(input::InputSystem* input_system) override
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(HasImgui())
            {
                ImGui_ImplSDL2_ProcessEvent(&e);
            }

            platform.OnEvent(input_system, e, [&](u32 id) {return windows[id]->size;});

            switch(e.type)
            {
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto& window = windows[e.window.windowID];
                    window->OnResized({e.window.data1, e.window.data2});
                }
                break;
            case SDL_QUIT:
                running = false;
                break;
            break;
            default:
                // ignore other events
                break;
            }
        }
    }
};

std::unique_ptr<Windows> Setup()
{
    constexpr Uint32 flags =
          SDL_INIT_VIDEO
        | SDL_INIT_JOYSTICK
        | SDL_INIT_GAMECONTROLLER
        | SDL_INIT_HAPTIC
        ;
    if(SDL_Init(flags) != 0)
    {
        LOG_ERROR("Unable to initialize SDL: {}", SDL_GetError());
        return nullptr;
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

    return std::make_unique<WindowsImpl>();
}


int MainLoop(input::UnitDiscovery discovery, std::unique_ptr<Windows>&& windows, input::InputSystem* input_system, UpdateFunction&& on_update)
{
    auto last = SDL_GetPerformanceCounter();

    while(windows->running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        windows->PumpEvents(input_system);
        input_system->UpdatePlayerConnections(discovery, windows->GetInputPlatform());
        if(false == on_update(dt))
        {
            return 0;
        }
        windows->Render();
    }

    return 0;
}
