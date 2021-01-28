#include "tred/windows.h"

#include <map>
#include <string_view>
#include <string>
#include <array>
#include <set>

#include "SDL.h"
#include "glad/glad.h"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "tred/log.h"
#include "tred/opengl.debug.h"
#include "tred/types.h"
#include "tred/handle.h"

#include "tred/input/system.h"
#include "tred/input/platform.h"

#include "tred/windows.sdl.convert.h"
#include "tred/windows.sdl.joystick.h"


using WindowId = Uint32;


struct SdlPlatform : public input::Platform
{
    SdlPlatform()
    {
        const auto number_of_joysticks = SDL_NumJoysticks();

        LOG_INFO("Joysticks found: {}", number_of_joysticks);
        for (int i = 0; i < number_of_joysticks; ++i)
        {
            sdl::LogInfoAboutJoystick(i);
            AddJoystickFromDevice(i);
        }
    }

    void OnEvent(input::InputSystem* system, const SDL_Event& event, std::function<glm::ivec2 (u32)> window_size)
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                system->OnKeyboardKey(sdl::ToKey(event.key.keysym), event.type == SDL_KEYDOWN);
                break;

            case SDL_JOYAXISMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jaxis.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    system->OnJoystickAxis(found->second, event.jaxis.axis, event.jaxis.value/-32768.0f);
                }
                break;

            case SDL_JOYBALLMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jball.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    system->OnJoystickBall(found->second, input::Axis::X, event.jball.ball, event.jball.xrel);
                    system->OnJoystickBall(found->second, input::Axis::Y, event.jball.ball, event.jball.yrel);
                }
                break;

            case SDL_JOYHATMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jhat.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    const auto hat = sdl::GetHatValues(event.jhat.value);
                    system->OnJoystickHat(found->second, input::Axis::X, event.jhat.hat, static_cast<float>(hat.x));
                    system->OnJoystickHat(found->second, input::Axis::Y, event.jhat.hat, static_cast<float>(hat.y));
                }
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                {
                    const auto down = event.type == SDL_JOYBUTTONDOWN;
                    const auto joystick_button = event.jbutton.button;

                    auto found = sdljoystick_to_id.find(event.jbutton.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    const auto joystick_id = found->second;
                    system->OnJoystickButton(joystick_id, joystick_button, down);
                }
                break;

            case SDL_JOYDEVICEADDED:
                AddJoystickFromDevice(event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                AddJoystickFromDevice(event.jdevice.which);
                break;

            case SDL_MOUSEMOTION:
                {
                    const auto size = window_size(event.motion.windowID);
                    auto pos_to_abs = [](int p, int s) -> float
                    {
                        // normalize to [0 1] range
                        const auto r = static_cast<float>(p)/static_cast<float>(s);

                        // scale to to [-1 +1] range
                        return (r - 0.5f) * 2.0f;
                    };
                    system->OnMouseAxis(input::Axis::X, static_cast<float>(event.motion.xrel), pos_to_abs(event.motion.x, size.x));
                    system->OnMouseAxis(input::Axis::Y, static_cast<float>(event.motion.yrel), pos_to_abs(event.motion.y, size.y));
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                // old-style cast error: ignore for now
                // if(event.button.which != SDL_TOUCH_MOUSEID)
                {
                    system->OnMouseButton(sdl::ToMouseButton(event.button.button), event.type == SDL_MOUSEBUTTONDOWN);
                }
                break;

            case SDL_MOUSEWHEEL:
                {
                    const auto direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                    if(event.wheel.x != 0)
                    {
                        system->OnMouseWheel(input::Axis::X, static_cast<float>(event.wheel.x) * direction);
                    }
                    if(event.wheel.y != 0)
                    {
                        system->OnMouseWheel(input::Axis::Y, static_cast<float>(event.wheel.y) * direction);
                    }
                }
                break;

            default:
                return;
        }
    }


    std::vector<input::JoystickId> ActiveAndFreeJoysticks()
    {
        auto r = std::vector<input::JoystickId>{};

        for(auto joy: joysticks.AsPairs())
        {
            if(joy.second.in_use == false)
            {
                r.emplace_back(joy.first);
            }
        }

        return r;
    }

    struct JoystickData
    {
        std::unique_ptr<sdl::Joystick> joystick;
        SDL_JoystickID instance_id;
        bool in_use = false;
    };
    using JoystickFunctions = HandleFunctions64<input::JoystickId>;
    HandleVector<JoystickData, JoystickFunctions> joysticks;
    std::map<SDL_JoystickID, input::JoystickId> sdljoystick_to_id;

    input::JoystickId AddJoystickFromDevice(int device_id)
    {
        const auto id = joysticks.Add();

        joysticks[id].joystick = std::make_unique<sdl::Joystick>(device_id);
        joysticks[id].instance_id = joysticks[id].joystick->GetDeviceIndex();
        joysticks[id].in_use = false;

        LOG_INFO("Added a joystick named {}", joysticks[id].joystick->GetName());

        return id;
    }

    void RemoveJoystickFromInstance(SDL_JoystickID instance_id)
    {
        const auto found_id = sdljoystick_to_id.find(instance_id);
        if(found_id == sdljoystick_to_id.end())
        {
            LOG_WARNING("Unable to remove {}", instance_id);
            return;
        }
        const auto id = found_id->second;
        // LOG_INFO("Removed joystick: {}", joysticks[id].joystick->GetName());
        LOG_INFO("Removed joystick (todo): add name here");
        joysticks.Remove(id);
        sdljoystick_to_id.erase(instance_id);
    }

    bool MatchUnit(input::JoystickId joy, const std::string& unit)
    {
        return joysticks[joy].joystick->GetGuid() == unit;
    }

    void StartUsing(input::JoystickId joy)
    {
        joysticks[joy].in_use = true;
    }
};


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
    SdlPlatform platform;

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
