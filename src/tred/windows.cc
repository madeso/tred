#include "tred/windows.h"

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
#include "tred/undef_windows.h"

#include "tred/log.h"
#include "tred/opengl.debug.h"
#include "tred/types.h"
#include "tred/handle.h"

#include "tred/input/system.h"
#include "tred/input/platform.h"

#include "tred/windows.sdl.convert.h"
#include "tred/windows.sdl.joystick.h"


#define DIE(x) assert(false && x);


using window_id = Uint32;


namespace
{
    // doesn't really seem necessary as sdl will detect them anyway?
    // todo(Gustav): remove this
    constexpr bool log_joysticks_at_startup = false;

    // auto enable relative_mouse
    // default should be true? but it's useful to set it to false to help with debugging
    constexpr bool relative_mouse = false;

    constexpr bool log_joystick_connection_events = true;
}


struct gamecontroller_data
{
    std::unique_ptr<sdl::game_controller> controller;
    sdl::gamecontroller_state last_state;
};


enum class joystick_detection_state
{
    joystick, gamecontroller, not_detected
};


struct joystick_data
{
    std::unique_ptr<sdl::joystick> joystick;
    std::unique_ptr<gamecontroller_data> gamecontroller;
    SDL_JoystickID instance_id = 0;
    joystick_detection_state in_use = joystick_detection_state::not_detected;
};


struct sdl_platform : public input::platform
{
    sdl_platform()
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

    void OnEventsCompleted(input::input_system* system)
    {
        if(mouse_motion_this_frame == false && mouse_motion_last_frame == true)
        {
            // we got motion last frame but not this frame
            // ... then reset the motion to 0,0
            OnMouseRelativeAxis(system, 0.0f, 0.0f);
        }
        mouse_motion_last_frame = mouse_motion_this_frame;
        mouse_motion_this_frame = false;
    }


    void OnMouseRelativeAxis(input::input_system* system, float dx, float dy)
    {
        system->on_mouse_axis(input::xy_axis::x, dx, last_mouse_x);
        system->on_mouse_axis(input::xy_axis::y, dy, last_mouse_y);
    }

    void OnEvent(std::vector<input::joystick_id>* lost_joysticks, input::input_system* system, const SDL_Event& event, std::function<glm::ivec2 (u32)> window_size)
    {
        std::optional<input::joystick_id> detected_controller;
        auto should_handle_joystick = [this, &detected_controller](input::joystick_id id) -> bool
        {
            switch(joysticks[id].in_use)
            {
            case joystick_detection_state::gamecontroller:
                detected_controller = id;
                return false;
            case joystick_detection_state::joystick:
                return true;
            case joystick_detection_state::not_detected:
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
                        system->on_joystick_ball(found->second, input::xy_axis::x, event.jball.ball, event.jball.xrel);
                        system->on_joystick_ball(found->second, input::xy_axis::y, event.jball.ball, event.jball.yrel);
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
                        system->on_joystick_hat(found->second, input::xy_axis::x, event.jhat.hat, static_cast<float>(hat.x));
                        system->on_joystick_hat(found->second, input::xy_axis::y, event.jhat.hat, static_cast<float>(hat.y));
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
                AddJoystickFromDevice(event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                if(log_joystick_connection_events)
                {
                    LOG_INFO("SDL: Joystick removed");
                }
                RemoveJoystickFromInstance(system, lost_joysticks, event.jdevice.which);
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
                    last_mouse_x = pos_to_abs(event.motion.x, size.x);
                    last_mouse_y = pos_to_abs(event.motion.y, size.y);
                    OnMouseRelativeAxis(system, static_cast<float>(event.motion.xrel), static_cast<float>(-event.motion.yrel));
                    mouse_motion_this_frame = true;
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
                        system->on_mouse_wheel(input::xy_axis::x, static_cast<float>(event.wheel.x) * direction);
                    }
                    if(event.wheel.y != 0)
                    {
                        system->on_mouse_wheel(input::xy_axis::y, static_cast<float>(event.wheel.y) * direction);
                    }
                }
                break;

            default:
                return;
        }

        // todo(Gustav): move to the end of event loop so we don't iterate controlls for each event
        if(detected_controller)
        {
            SendEventsForGameController(system, *detected_controller);
        }
    }

    void SendEventsForGameController(input::input_system* system, input::joystick_id detected_controller)
    {
        gamecontroller_data* controller = joysticks[detected_controller].gamecontroller.get();
        const auto new_state = sdl::gamecontroller_state::get_state(controller->controller.get());
        auto& old_state = controller->last_state;

        for(auto button: sdl::valid_buttons)
        {
            const auto id = static_cast<size_t>(button);
            if(old_state.buttons[id] != new_state.buttons[id])
            {
                const auto my_button = sdl::to_controller_button(button);
                if(my_button != input::gamecontroller_button::invalid)
                {
                    system->on_gamecontroller_button(detected_controller, my_button, new_state.buttons[id] ? 1.0f : 0.0f);
                }
            }
        }

        for(auto axis: sdl::valid_axes)
        {
            const auto id = static_cast<size_t>(axis);
            if(old_state.axes[id] != new_state.axes[id])
            {
                // "The state is a value ranging from -32768 to 32767." so make sure it falls in the -1 to +1 range
                const float state_abs = std::min(1.0f, std::abs(static_cast<float>(new_state.axes[id])/32767.0f));
                const float state_sign = (new_state.axes[id] < 0 ? -1.0f:1.0f);
                const float state = state_abs * state_sign;

                const auto my_button = sdl::to_controller_button(axis);
                if(my_button != input::gamecontroller_button::invalid)
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


    std::vector<input::joystick_id> get_active_and_free_joysticks() override
    {
        auto r = std::vector<input::joystick_id>{};

        for(auto joy: joysticks.as_pairs())
        {
            if(joy.second.in_use == joystick_detection_state::not_detected)
            {
                r.emplace_back(joy.first);
            }
        }

        return r;
    }

    using JoystickFunctions = handle_functions64<input::joystick_id>;
    handle_vector<joystick_data, JoystickFunctions> joysticks;
    std::map<SDL_JoystickID, input::joystick_id> sdljoystick_to_id;

    input::joystick_id AddJoystickFromDevice(int device_id)
    {
        if(log_joystick_connection_events)
        {
            LOG_INFO("Adding joystick from device {}", device_id);
        }
        auto joystick = std::make_unique<sdl::joystick>(device_id);
        const auto instance_id = joystick->get_device_index();

        auto found = sdljoystick_to_id.find(instance_id);
        if(found != sdljoystick_to_id.end())
        {
            LOG_WARNING("Tried to add joystick that was already added!");
        }

        const auto id = joysticks.create_new_handle();
        {
            auto controller = std::make_unique<gamecontroller_data>();
            controller->controller = std::make_unique<sdl::game_controller>(device_id);
            if(controller->controller->is_valid())
            {
                joysticks[id].gamecontroller = std::move(controller);
            }
        }
        joysticks[id].joystick = std::move(joystick);
        joysticks[id].instance_id = instance_id;
        joysticks[id].in_use = joystick_detection_state::not_detected;

        // overwrite existing (if any) sdl joystick instance_id with the new id
        sdljoystick_to_id[instance_id] = id;

        LOG_INFO("Added a joystick named {} gamecontroller: {}", joysticks[id].joystick->get_name(), joysticks[id].gamecontroller != nullptr);
        return id;

    }

    void RemoveJoystickFromInstance(input::input_system* system, std::vector<input::joystick_id>* lost_joysticks, SDL_JoystickID instance_id)
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
        joysticks[id].in_use = joystick_detection_state::not_detected;
        lost_joysticks->push_back(id);
        joysticks.mark_for_reuse(id);
        sdljoystick_to_id.erase(instance_id);
    }

    bool match_unit(input::joystick_id joy, const std::string& unit) override
    {
        return joysticks[joy].joystick->get_guid() == unit;
    }

    void start_using(input::joystick_id joy, input::joystick_type type) override
    {
        if(log_joystick_connection_events)
        {
            LOG_INFO("Started using {}", type == input::joystick_type::joystick ? "joystick" : "gamecontroller");
        }
        joysticks[joy].in_use = type == input::joystick_type::game_controller ? joystick_detection_state::gamecontroller : joystick_detection_state::joystick;
    }

    void on_changed_connection(const std::string& new_connection) override
    {
        LOG_INFO("Player started using {}", new_connection);
    }
};

namespace
{
    void setup_open_gl(SDL_Window* window, SDL_GLContext glcontext, bool imgui)
    {
        setup_opengl_debug();
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
}


struct window_implementation : public detail::window
{
    std::string title;
    glm::ivec2 size;
    render_function on_render;
    std::optional<imgui_function> imgui;

    SDL_Window* sdl_window;
    SDL_GLContext sdl_glcontext;

    window_implementation(const std::string& t, const glm::ivec2& s, render_function&& r, std::optional<imgui_function>&& i)
        : title(t)
        , size(s)
        , on_render(r)
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

        setup_open_gl(sdl_window, sdl_glcontext, imgui.has_value());
    }

    ~window_implementation() override
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

    void render() override
    {
        if(sdl_window == nullptr) { return; }

        on_render(size);

        if(imgui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(sdl_window);
            ImGui::NewFrame();

            (*imgui)();

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

    [[nodiscard]] bool has_imgui() const
    {
        return imgui.has_value();
    }
};


void windows::add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render)
{
    add_window_implementation(title, size, std::move(on_render), std::nullopt);
}


void windows::add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render, imgui_function&& on_imgui)
{
    add_window_implementation(title, size, std::move(on_render), std::move(on_imgui));
}


struct windows_implementation : public windows
{
    std::map<window_id, std::unique_ptr<window_implementation>> windows;
    std::unique_ptr<sdl_platform> platform;

    explicit windows_implementation()
        : platform(std::make_unique<sdl_platform>())
    {
    }

    ~windows_implementation()
    {
        windows.clear();
        platform.reset();
        SDL_Quit();
    }

    void add_window_implementation(const std::string& title, const glm::ivec2& size, render_function&& on_render, std::optional<imgui_function>&& imgui) override
    {
        auto window = std::make_unique<window_implementation>(title, size, std::move(on_render), std::move(imgui));
        windows[window->get_id()] = std::move(window);
    }

    void render() override
    {
        for(auto& window: windows)
        {
            window.second->render();
        }
    }

    bool has_imgui()
    {
        for(auto& window: windows)
        {
            if(window.second->has_imgui())
            {
                return true;
            }
        }

        return false;
    }

    input::platform* get_input_platform() override
    {
        return platform.get();
    }

    void pump_events(input::input_system* input_system) override
    {
        std::vector<input::joystick_id> lost_joysticks;
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(has_imgui())
            {
                ImGui_ImplSDL2_ProcessEvent(&e);
            }

            platform->OnEvent(&lost_joysticks, input_system, e, [&](u32 id) {return windows[id]->size;});

            switch(e.type)
            {
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto found = windows.find(e.window.windowID);
                    if(found != windows.end())
                    {
                        found->second->on_resized({e.window.data1, e.window.data2});
                    }
                    else
                    {
                        LOG_INFO("Recieved resize for non-existant window: {}", e.window.windowID);
                    }
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

        platform->OnEventsCompleted(input_system);
    }
};


std::unique_ptr<windows> setup()
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


    // todo(Gustav): need to possible check input if this is required (or enable it later when required)
    if(relative_mouse)
    {
        const auto relative_enabled = SDL_SetRelativeMouseMode(SDL_TRUE) >= 0;
        if(relative_enabled == false)
        {
            LOG_ERROR("Unable to set relative mouse");
            return nullptr;
        }
    }

    return std::make_unique<windows_implementation>();
}


int main_loop(input::unit_discovery discovery, std::unique_ptr<windows>&& windows, input::input_system* input_system, update_function&& on_update)
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
