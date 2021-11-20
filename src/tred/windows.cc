#include "tred/windows.h"

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
#include "tred/undef_windows.h"

#include "tred/dependency_sdl.h"
#include "tred/log.h"
#include "tred/opengl_utils.h"
#include "tred/opengl_state.h"
#include "tred/types.h"
#include "tred/handle.h"
#include "tred/render2.h"
#include "tred/layer2.h"

#include "tred/input/system.h"
#include "tred/input/platform.h"

#include "tred/handle.h"

#include "tred/windows.sdl.convert.h"
#include "tred/windows.sdl.joystick.h"


#define DIE(x) assert(false && x);


using window_id = Uint32;


namespace
{
    // doesn't really seem necessary as sdl will detect them anyway?
    // todo(Gustav): remove this
    constexpr bool log_joysticks_at_startup = false;

    constexpr bool log_joystick_connection_events = true;

    constexpr SDL_Keycode imgui_toggle_key = SDLK_F1;
}


struct GamecontrollerData
{
    std::unique_ptr<sdl::GameController> controller;
    sdl::GamecontrollerState last_state;
};


enum class JoystickDetectionState
{
    joystick, gamecontroller, not_detected
};


struct JoystickData
{
    std::unique_ptr<sdl::Joystick> joystick;
    std::unique_ptr<GamecontrollerData> gamecontroller;
    SDL_JoystickID instance_id = 0;
    JoystickDetectionState in_use = JoystickDetectionState::not_detected;
};


struct SdlPlatform : public input::Platform
{
    SdlPlatform()
    {
        if(log_joysticks_at_startup)
        {
            const auto number_of_joysticks = SDL_NumJoysticks();
            LOG_INFO("Joysticks found: {}", number_of_joysticks);
            for (int i = 0; i < number_of_joysticks; ++i)
            {
                sdl::log_info_about_joystick(i);
            }
        }
    }

    bool mouse_motion_last_frame = false;
    bool mouse_motion_this_frame = false;
    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    void on_events_completed(input::InputSystem* system)
    {
        if(mouse_motion_this_frame == false && mouse_motion_last_frame == true)
        {
            // we got motion last frame but not this frame
            // ... then reset the motion to 0,0
            on_mouse_relative_axis(system, 0.0f, 0.0f);
        }
        mouse_motion_last_frame = mouse_motion_this_frame;
        mouse_motion_this_frame = false;
    }


    void on_mouse_relative_axis(input::InputSystem* system, float dx, float dy)
    {
        system->on_mouse_axis(input::Axis2::x, dx, last_mouse_x);
        system->on_mouse_axis(input::Axis2::y, dy, last_mouse_y);
    }

    void on_event(std::vector<input::JoystickId>* lost_joysticks, input::InputSystem* system, const SDL_Event& event, std::function<std::optional<glm::ivec2> (u32)> window_size)
    {
        std::optional<input::JoystickId> detected_controller;
        auto should_handle_joystick = [this, &detected_controller](input::JoystickId id) -> bool
        {
            switch(joysticks[id].in_use)
            {
            case JoystickDetectionState::gamecontroller:
                detected_controller = id;
                return false;
            case JoystickDetectionState::joystick:
                return true;
            case JoystickDetectionState::not_detected:
                if(joysticks[id].gamecontroller)
                {
                    detected_controller = id;
                }
                return true;
            default:
                DIE("Unhandled case");
                return false;
            }
        };

        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                system->on_keyboard_key(sdl::to_keyboard_key(event.key.keysym), event.type == SDL_KEYDOWN);
                break;

            case SDL_JOYAXISMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jaxis.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    if(should_handle_joystick(found->second))
                    {
                        system->on_joystick_axis(found->second, event.jaxis.axis, event.jaxis.value/-32768.0f);
                    }
                }
                break;

            case SDL_JOYBALLMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jball.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    if(should_handle_joystick(found->second))
                    {
                        system->on_joystick_ball(found->second, input::Axis2::x, event.jball.ball, event.jball.xrel);
                        system->on_joystick_ball(found->second, input::Axis2::y, event.jball.ball, event.jball.yrel);
                    }
                }
                break;

            case SDL_JOYHATMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jhat.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    if(should_handle_joystick(found->second))
                    {
                        const auto hat = sdl::get_hat_values(event.jhat.value);
                        system->on_joystick_hat(found->second, input::Axis2::x, event.jhat.hat, static_cast<float>(hat.x));
                        system->on_joystick_hat(found->second, input::Axis2::y, event.jhat.hat, static_cast<float>(hat.y));
                    }
                }
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                {
                    const auto down = event.type == SDL_JOYBUTTONDOWN;
                    const auto joystick_button = event.jbutton.button;

                    auto found = sdljoystick_to_id.find(event.jbutton.which);
                    if(found == sdljoystick_to_id.end())
                    {
                        LOG_INFO("Button ignored due to invalid joystick");
                        return;
                    }
                    if(should_handle_joystick(found->second))
                    {
                        const auto joystick_id = found->second;
                        system->on_joystick_button(joystick_id, joystick_button, down);
                    }
                }
                break;

            case SDL_JOYDEVICEADDED:
                if(log_joystick_connection_events)
                {
                    LOG_INFO("SDL: joystick added");
                }
                add_joystick_from_device(event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                if(log_joystick_connection_events)
                {
                    LOG_INFO("SDL: Joystick removed");
                }
                remove_joystick_from_instance(system, lost_joysticks, event.jdevice.which);
                break;

            case SDL_MOUSEMOTION:
                {
                    const auto size = window_size(event.motion.windowID);

                    if(size)
                    {
                        auto pos_to_abs = [](int p, int s) -> float
                        {
                            // normalize to [0 1] range
                            const auto r = static_cast<float>(p)/static_cast<float>(s);

                            // scale to to [-1 +1] range
                            return (r - 0.5f) * 2.0f;
                        };
                        last_mouse_x = pos_to_abs(event.motion.x, size->x);
                        last_mouse_y = pos_to_abs(event.motion.y, size->y);
                        on_mouse_relative_axis(system, static_cast<float>(event.motion.xrel), static_cast<float>(-event.motion.yrel));
                        mouse_motion_this_frame = true;
                    }
                    else
                    {
                        LOG_INFO("ignored invalid window");
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                // old-style cast error: ignore for now
                // if(event.button.which != SDL_TOUCH_MOUSEID)
                {
                    system->on_mouse_button(sdl::to_mouse_button(event.button.button), event.type == SDL_MOUSEBUTTONDOWN);
                }
                break;

            case SDL_MOUSEWHEEL:
                {
                    const auto direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                    if(event.wheel.x != 0)
                    {
                        system->on_mouse_wheel(input::Axis2::x, static_cast<float>(event.wheel.x) * direction);
                    }
                    if(event.wheel.y != 0)
                    {
                        system->on_mouse_wheel(input::Axis2::y, static_cast<float>(event.wheel.y) * direction);
                    }
                }
                break;

            default:
                return;
        }

        // todo(Gustav): move to the end of event loop so we don't iterate controlls for each event
        if(detected_controller)
        {
            send_events_for_game_controller(system, *detected_controller);
        }
    }

    void send_events_for_game_controller(input::InputSystem* system, input::JoystickId detected_controller)
    {
        GamecontrollerData* controller = joysticks[detected_controller].gamecontroller.get();
        const auto new_state = sdl::GamecontrollerState::get_state(controller->controller.get());
        auto& old_state = controller->last_state;

        for(auto button: sdl::valid_buttons)
        {
            const auto id = static_cast<std::size_t>(button);
            if(old_state.buttons[id] != new_state.buttons[id])
            {
                const auto my_button = sdl::to_controller_button(button);
                if(my_button != input::GamecontrollerButton::invalid)
                {
                    system->on_gamecontroller_button(detected_controller, my_button, new_state.buttons[id] ? 1.0f : 0.0f);
                }
            }
        }

        for(auto axis: sdl::valid_axes)
        {
            const auto id = static_cast<std::size_t>(axis);
            if(old_state.axes[id] != new_state.axes[id])
            {
                // "The state is a value ranging from -32768 to 32767." so make sure it falls in the -1 to +1 range
                const float state_abs = std::min(1.0f, std::abs(static_cast<float>(new_state.axes[id])/32767.0f));
                const float state_sign = (new_state.axes[id] < 0 ? -1.0f:1.0f);
                const float state = state_abs * state_sign;

                const auto my_button = sdl::to_controller_button(axis);
                if(my_button != input::GamecontrollerButton::invalid)
                {
                    // special case for 'triggers'
                    // a trigger is axis in sdl but considered buttons in input system
                    system->on_gamecontroller_button(detected_controller, my_button, state);
                }
                else
                {
                    const auto my_axis = sdl::to_controller_axis(axis);
                    system->on_gamecontroller_axis(detected_controller, my_axis, state);
                }
            }
        }

        old_state = new_state;
    }


    std::vector<input::JoystickId> get_active_and_free_joysticks() override
    {
        auto r = std::vector<input::JoystickId>{};

        for(auto joy: joysticks.as_pairs())
        {
            if(joy.second.in_use == JoystickDetectionState::not_detected)
            {
                r.emplace_back(joy.first);
            }
        }

        return r;
    }

    using JoystickFunctions = HandleFunctions64<input::JoystickId>;
    HandleVector<JoystickData, JoystickFunctions> joysticks;
    std::map<SDL_JoystickID, input::JoystickId> sdljoystick_to_id;

    input::JoystickId add_joystick_from_device(int device_id)
    {
        if(log_joystick_connection_events)
        {
            LOG_INFO("Adding joystick from device {}", device_id);
        }
        auto joystick = std::make_unique<sdl::Joystick>(device_id);
        const auto instance_id = joystick->get_device_index();

        auto found = sdljoystick_to_id.find(instance_id);
        if(found != sdljoystick_to_id.end())
        {
            LOG_WARNING("Tried to add joystick that was already added!");
        }

        std::unique_ptr<GamecontrollerData> gamecontroller;
        {
            auto controller = std::make_unique<GamecontrollerData>();
            controller->controller = std::make_unique<sdl::GameController>(device_id);
            if(controller->controller->is_valid())
            {
                gamecontroller = std::move(controller);
            }
        }
        const auto id = joysticks.add
        (
            {
                std::move(joystick),
                std::move(gamecontroller),
                instance_id,
                JoystickDetectionState::not_detected
            }
        );
        joysticks[id].joystick = std::move(joystick);
        joysticks[id].instance_id = instance_id;
        joysticks[id].in_use = JoystickDetectionState::not_detected;

        // overwrite existing (if any) sdl joystick instance_id with the new id
        sdljoystick_to_id[instance_id] = id;

        LOG_INFO("Added a joystick named {} gamecontroller: {}", joysticks[id].joystick->get_name(), joysticks[id].gamecontroller != nullptr);
        return id;

    }

    void remove_joystick_from_instance(input::InputSystem* system, std::vector<input::JoystickId>* lost_joysticks, SDL_JoystickID instance_id)
    {
        if(log_joystick_connection_events)
        {
            LOG_INFO("Removing joystick from instance");
        }
        const auto found_id = sdljoystick_to_id.find(instance_id);
        if(found_id == sdljoystick_to_id.end())
        {
            LOG_WARNING("Unable to remove {}", instance_id);
            return;
        }
        const auto id = found_id->second;
        LOG_INFO("Removed joystick (todo): add name here");
        system->on_joystick_lost(id);
        if(joysticks[id].gamecontroller)
        {
            system->on_gamecontroller_lost(id);
        }
        joysticks[id].joystick.reset();
        joysticks[id].gamecontroller.reset();
        joysticks[id].in_use = JoystickDetectionState::not_detected;
        lost_joysticks->push_back(id);
        joysticks.remove(id);
        sdljoystick_to_id.erase(instance_id);
    }

    bool match_unit(input::JoystickId joy, const std::string& unit) override
    {
        return joysticks[joy].joystick->get_guid() == unit;
    }

    void start_using(input::JoystickId joy, input::JoystickType type) override
    {
        if(log_joystick_connection_events)
        {
            LOG_INFO("Started using {}", type == input::JoystickType::joystick ? "joystick" : "gamecontroller");
        }
        joysticks[joy].in_use = type == input::JoystickType::game_controller ? JoystickDetectionState::gamecontroller : JoystickDetectionState::joystick;
    }

    void on_changed_connection(const std::string& new_connection) override
    {
        LOG_INFO("Player started using {}", new_connection);
    }
};

struct WindowImplementation;

namespace
{
    struct ImguiState
    {
        bool imgui_requested = true;

        std::optional<imgui_function> on_imgui;
        bool enabled = false;
        detail::Window* owning_window = nullptr;



        bool is_imgui(detail::Window* win)
        {
            return win == owning_window;
        }

        bool is_imgui_initialized()
        {
            return owning_window != nullptr;
        }
    };

    struct OpenGlSetup
    {
        ImguiState* imgui;
        bool opengl_initialized = false;
        std::unique_ptr<Render2> render_data;
        
        SDL_GLContext sdl_glcontext;
        WindowImplementation* active_window = nullptr; // sdl current active window
        ImGuiContext* imgui_context = nullptr;

        explicit OpenGlSetup(ImguiState* i)
            : imgui(i)
            , sdl_glcontext(nullptr)
        {
        }

        void run_setup(OpenglStates* states, SDL_Window* window, SDL_GLContext glcontext, detail::Window* win, bool is_main)
        {
            if(opengl_initialized == false)
            {
                opengl_setup(states);
                opengl_set3d(states);

                const auto* renderer = glGetString(GL_RENDERER); // get renderer string
                const auto* version = glGetString(GL_VERSION); // version as a string
                LOG_INFO("Renderer: {}", renderer);
                LOG_INFO("Version: {}", version);

                render_data = std::make_unique<Render2>();

                opengl_initialized = true;
            }

            if(is_main && imgui->imgui_requested)
            {
                assert(imgui->owning_window == nullptr);

                IMGUI_CHECKVERSION();
                imgui_context = ImGui::CreateContext();
                auto& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

                ImGui::StyleColorsLight();

                ImGui_ImplSDL2_InitForOpenGL(window, glcontext);

                const char* glsl_version = "#version 130";
                ImGui_ImplOpenGL3_Init(glsl_version);

                imgui->owning_window = win;
            }
        }

        void closing_window(detail::Window* win)
        {
            if(win == imgui->owning_window)
            {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplSDL2_Shutdown();
                imgui->owning_window = nullptr;
            }
        }

        ~OpenGlSetup()
        {
            assert(imgui->owning_window == nullptr);
        }
    };
}


struct WindowImplementation : public detail::Window
{
    std::string title;
    glm::ivec2 size;

    SDL_Window* sdl_window;

    OpenGlSetup* opengl_setup;

    std::optional<render_function> on_render;
    ImguiState* imgui_state;

    OpenglStates* states;

    bool is_main;

    WindowImplementation(OpenglStates* st, const std::string& t, const glm::ivec2& s, OpenGlSetup* setup, ImguiState* ist, bool im)
        : title(t)
        , size(s)
        , sdl_window(nullptr)
        , opengl_setup(setup)
        , imgui_state(ist)
        , states(st)
        , is_main(im)
    {
        LOG_INFO("Creating window {}", title);
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

        if(opengl_setup->sdl_glcontext == nullptr)
        {
            opengl_setup->sdl_glcontext = SDL_GL_CreateContext(sdl_window);

            if(opengl_setup->sdl_glcontext == nullptr)
            {
                LOG_ERROR("Could not create window: {}", SDL_GetError());

                SDL_DestroyWindow(sdl_window);
                sdl_window = nullptr;

                return;
            }

            if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
            {
                LOG_ERROR("Failed to initialize OpenGL context");

                SDL_GL_DeleteContext(opengl_setup->sdl_glcontext);
                opengl_setup->sdl_glcontext = nullptr;

                SDL_DestroyWindow(sdl_window);
                sdl_window = nullptr;

                return;
            }

            setup->run_setup(states, sdl_window, opengl_setup->sdl_glcontext, this, is_main);
        }
    }

    ~WindowImplementation() override
    {
        if(sdl_window)
        {
            opengl_setup->closing_window(this);

            if(is_main)
            {
                SDL_GL_DeleteContext(opengl_setup->sdl_glcontext);
                opengl_setup->sdl_glcontext = nullptr;
            }
            SDL_DestroyWindow(sdl_window);
        }
    }

    WindowImplementation(const WindowImplementation&) = delete;
    void operator=(const WindowImplementation&) = delete;

    WindowImplementation(WindowImplementation&&) = delete;
    void operator=(WindowImplementation&&) = delete;

    void activate_this_window()
    {
        if(this == opengl_setup->active_window) { return; }
        
        opengl_setup->active_window = this;

        [[maybe_unused]] const auto error = SDL_GL_MakeCurrent
        (
            sdl_window,
            opengl_setup->sdl_glcontext
        );

        // todo(Gustav): log error or do some error handling?
        assert(error == 0);

        if(imgui_state->on_imgui && imgui_state->is_imgui(this))
        {
            assert(opengl_setup->imgui_context);
            ImGui::SetCurrentContext(opengl_setup->imgui_context);
        }
    }

    void render() override
    {
        if(sdl_window == nullptr) { return; }

        activate_this_window();

        if(on_render)
        {
            (*on_render)({states, opengl_setup->render_data.get(), size});
        }

        if(imgui_state->on_imgui && imgui_state->is_imgui(this))
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(sdl_window);
            ImGui::NewFrame();

            (*imgui_state->on_imgui)();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(sdl_window);
    }

    void on_resized(const glm::ivec2& new_size)
    {
        size = new_size;
    }

    [[nodiscard]] window_id get_id() const
    {
        if(sdl_window == nullptr) { return 0; }
        return SDL_GetWindowID(sdl_window);
    }
};


enum class MouseState
{
    unknown, locked, free
};


struct WindowsImplementation : public Windows
{
    HandleVector<std::unique_ptr<WindowImplementation>, HandleFunctions64<WindowHandle>> windows;
    std::map<window_id, WindowHandle> window_id_to_handle;
    std::unique_ptr<SdlPlatform> platform;
    ImguiState imgui_state;
    OpenGlSetup opengl_setup;
    OpenglStates states;
    MouseState mouse_state = MouseState::unknown;

    explicit WindowsImplementation()
        : platform(std::make_unique<SdlPlatform>())
        , opengl_setup(&imgui_state)
    {
    }

    ~WindowsImplementation()
    {
        windows.clear();
        platform.reset();
        SDL_Quit();
    }

    WindowImplementation* find_main_window_or_null()
    {
        for(auto& window: windows)
        {
            if(window->is_main)
            {
                return window.get();
            }
        }

        return nullptr;
    }

    WindowHandle add_window(const std::string& title, const glm::ivec2& size) override
    {
        WindowImplementation* main_window = find_main_window_or_null();
        const auto is_main = main_window == nullptr;
        auto window = windows.add(std::make_unique<WindowImplementation>(&states, title, size, &opengl_setup, &imgui_state, is_main));
        window_id_to_handle[windows[window]->get_id()] = window;

        if(is_main == false)
        {
            main_window->activate_this_window();
        }

        return window;
    }

    void set_render(WindowHandle window, render_function&& on_render) override
    {
        windows[window]->on_render = on_render;
    }

    void set_imgui(imgui_function&& on_render) override
    {
        imgui_state.on_imgui = on_render;
    }

    bool has_any_imgui() const
    {
        return imgui_state.on_imgui.has_value();
    }

    void render() override
    {
        for(auto& window: windows)
        {
            window->render();
        }

        (*windows.begin())->activate_this_window();
    }

    input::Platform* get_input_platform() override
    {
        return platform.get();
    }

    void pump_events(input::InputSystem* input_system) override
    {
        std::vector<input::JoystickId> lost_joysticks;
        SDL_Event e;
        const auto has_imgui = has_any_imgui();

        if(has_imgui)
        {
            ImGuiIO& io = ImGui::GetIO();

            // const int last_flags = io.ConfigFlags;

            io.ConfigFlags = io.ConfigFlags &
            ~(
                ImGuiConfigFlags_NavEnableKeyboard
                | ImGuiConfigFlags_NavEnableGamepad
                | ImGuiConfigFlags_NoMouseCursorChange
                | ImGuiConfigFlags_NavNoCaptureKeyboard
                | ImGuiConfigFlags_NoMouse
            );

            const int new_flags =
                imgui_state.enabled
                ? ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad
                : ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NavNoCaptureKeyboard
                ;
            io.ConfigFlags = io.ConfigFlags | new_flags;

            /*
            if(last_flags != io.ConfigFlags)
            {
                LOG_INFO("config flags changed from {} to {}", last_flags, io.ConfigFlags);
            }
            */
        }

        while(SDL_PollEvent(&e) != 0)
        {
            if(imgui_state.enabled)
            {
                if(imgui_state.is_imgui_initialized())
                {
                    ImGui_ImplSDL2_ProcessEvent(&e);
                }
            }

            if
            (
                has_imgui
                &&
                // capture both up and down key
                (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
                &&
                e.key.keysym.sym == imgui_toggle_key
            )
            {
                const auto down = e.type == SDL_KEYDOWN;
                if(!down)
                {
                    imgui_state.enabled = !imgui_state.enabled;
                }
            }
            else if(has_imgui && imgui_state.enabled)
            {
                // block all events to platform if in imgui state
            }
            else
            {
                platform->on_event
                (
                    &lost_joysticks,
                    input_system,
                    e,
                    [&](u32 id) -> std::optional<glm::ivec2>
                    {
                        const auto found = window_id_to_handle.find(id);
                        if(found == window_id_to_handle.end())
                        {
                            return std::nullopt;
                        }
                        else
                        {
                            return windows[found->second]->size;
                        }
                    }
                );
            }

            switch(e.type)
            {
            case SDL_WINDOWEVENT:
                switch(e.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        {
                            auto found = window_id_to_handle.find(e.window.windowID);
                            if(found != window_id_to_handle.end())
                            {
                                windows[found->second]->on_resized({e.window.data1, e.window.data2});
                            }
                            else
                            {
                                LOG_INFO("Recieved resize for non-existant window: {}", e.window.windowID);
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                    {
                            auto found = window_id_to_handle.find(e.window.windowID);
                            if(found != window_id_to_handle.end())
                            {
                                if(windows[found->second]->is_main)
                                {
                                    LOG_INFO("Closing main => closing app");
                                    running = false;
                                }
                                else
                                {
                                    LOG_INFO("Closing secondary => app is still running...");
                                    windows.remove(found->second);
                                    window_id_to_handle.erase(found);
                                }
                            }
                        }
                        break;
                }
                break;
            case SDL_QUIT:
                running = false;
                break;
            default:
                // ignore other events
                break;
            }
        }

        for(auto joystick: lost_joysticks)
        {
            input_system->on_joystick_lost(joystick);
        }

        platform->on_events_completed(input_system);

        const auto new_mouse_state = imgui_state.enabled == false && input_system->is_mouse_connected() ? MouseState::locked : MouseState::free;
        if(new_mouse_state != mouse_state)
        {
            // todo(Gustav): need to possible check input if this is required (or enable it later when required)
            const auto sdl_state = new_mouse_state == MouseState::locked ? SDL_TRUE : SDL_FALSE;
            const auto was_changed = SDL_SetRelativeMouseMode( sdl_state ) >= 0;
            if(was_changed == false)
            {
                LOG_ERROR("Unable to set relative mouse: {}", SDL_GetError());
            }
        }
    }
};


std::unique_ptr<Windows> setup()
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

    return std::make_unique<WindowsImplementation>();
}


int main_loop(input::unit_discovery discovery, std::unique_ptr<Windows>&& windows, input::InputSystem* input_system, update_function&& on_update)
{
    auto last = SDL_GetPerformanceCounter();

    while(windows->running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        windows->pump_events(input_system);
        input_system->update_player_connections(discovery, windows->get_input_platform());

        if(false == on_update(dt))
        {
            return 0;
        }
        windows->render();
    }

    return 0;
}
