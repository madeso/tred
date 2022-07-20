// standard headers
#include <string>
#include <string_view>
#include "tred/assert.h"
#include <vector>
#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>

#include "assets.h"

#include <fmt/core.h>

#include "tred/dependency_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


#include "tred/cint.h"
#include "tred/colors.h"
#include "tred/dependency_sdl.h"
#include "tred/handle.h"
#include "tred/hash.string.h"
#include "tred/input/config.h"
#include "tred/input/system.h"
#include "tred/input/table.h"
#include "tred/log.h"
#include "tred/stdutils.h"
#include "tred/windows.h"

#include "tred/render/render2.h"
#include "tred/render/render_func.h"
#include "tred/render/camera.h"
#include "tred/render/engine.h"
#include "tred/render/light.h"
#include "tred/render/world.h"
#include "tred/render/mesh.h"

// resource headers
#include "sprites/cards.h"

#define OLD_INPUT 0



constexpr unsigned int NUMBER_OF_POINT_LIGHTS = 4;
constexpr unsigned int NUMBER_OF_SPOT_LIGHTS = 1;
constexpr unsigned int NUMBER_OF_DIRECTIONAL_LIGHTS = 1;



float get_aspect_ratio(const Rectf& r)
{
    return r.get_width() / r.get_height();
}

float from01(float lower, float value, float upper)
{
    return value * (upper - lower) + lower;
}

float to01(float lower, float value, float upper)
{
    return (value - lower) / (upper - lower);
}

float remap(float ol, float ou, float f, float nl, float nu)
{
    const auto a = to01(ol, f, ou);
    return from01(nl, a, nu);
}


constexpr float plane_size = 20.0f;





#if OLD_INPUT == 0

float two_button_input(bool p, bool n)
{
    if(p && n) { return 0.0f;}
    if(p) { return 1.0f; }
    if(n) { return -1.0f; }
    return 0.0f;
}

struct DemoInput : input::InputSystemBase
{
    // if imgui is enabled, we shouldn't capture mouse if imgui is currently using it
    explicit DemoInput(bool imgui_is_enabled) : imgui_enabled(imgui_is_enabled) {}
    bool imgui_enabled;

    // "internal" state
    bool is_mouse_captured = false;

    // input
    bool move_left = false;
    bool move_right = false;
    bool move_up = false;
    bool move_down = false;
    bool move_fwd = false;
    bool move_back = false;
    bool should_quit = false;
    float mouse_dx = 0.0f;
    float mouse_dy = 0.0f;

    float get_inout() const { return two_button_input(move_fwd, move_back); }
    float get_leftright() const { return two_button_input(move_right, move_left); }
    float get_updown() const { return two_button_input(move_up, move_down); }

    static constexpr float sensitivity = 0.1f;
    float get_look_leftright() const { return mouse_dx * sensitivity; }
    float get_look_updown() const { return mouse_dy * sensitivity; }

    void update_player_connections(input::unit_discovery, input::Platform*) const override
    {
        // there is always one connection
    }

    bool is_mouse_connected() const override
    {
        return is_mouse_captured;
    }

    void on_keyboard_key(input::KeyboardKey key, bool down) override
    {
        if(is_mouse_captured == false)
        {
            if(down == false && key == input::KeyboardKey::escape)
            {
                should_quit = true;
            }

            // mouse is not captured - don't do fps controller
            move_left = false;
            move_right = false;
            move_up = false;
            move_down = false;
            move_fwd = false;
            move_back = false;
            return;
        }
        
        switch(key)
        {
        case input::KeyboardKey::a: move_left = down; break;
        case input::KeyboardKey::d: move_right = down; break;
        case input::KeyboardKey::w: move_fwd = down; break;
        case input::KeyboardKey::s: move_back = down; break;
        case input::KeyboardKey::space: move_up = down; break;
        case input::KeyboardKey::ctrl_left: move_down = down; break;
        default: break;
        }

        if(down == false && key == input::KeyboardKey::escape)
        {
            is_mouse_captured = false;
        }
    }

    void on_mouse_axis(input::Axis2 axis, float relative_state, float /*absolute_state*/) override
    {
        if(is_mouse_captured == false )
        {
            mouse_dx = 0.0f;
            mouse_dy = 0.0f;
            return;
        }

        switch(axis)
        {
            case input::Axis2::x: mouse_dx = relative_state; break;
            case input::Axis2::y: mouse_dy = relative_state; break;
            default: break;
        }
    }

    void on_mouse_wheel(input::Axis2, float) override
    {
        // don't care about mouse wheel
    }

    void on_mouse_button(input::MouseButton button, bool down) override
    {
        if(down) { return; }
        if(button != input::MouseButton::left) {return;}

        if(imgui_enabled && ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        is_mouse_captured = true;
    }

    // don't care about neither joystick nor gamepad
    void on_joystick_ball(input::JoystickId, input::Axis2, int, float) override {}
    void on_joystick_hat(input::JoystickId, input::Axis2, int, float) override {}
    void on_joystick_button(input::JoystickId, int, bool) override {}
    void on_joystick_axis(input::JoystickId, int, float) override {}
    void on_joystick_lost(input::JoystickId) override {}
    void on_gamecontroller_button(input::JoystickId, input::GamecontrollerButton, float) override {}
    void on_gamecontroller_axis(input::JoystickId, input::GamecontrollerAxis, float) override {}
    void on_gamecontroller_lost(input::JoystickId) override {}
};
#endif


bool ui_attenuation(render::Attenuation* a)
{
    bool changed = false;
    changed = ImGui::DragFloat("Attenuation constant", &a->constant, 0.01f) || changed;
    changed = ImGui::DragFloat("Attenuation linear", &a->linear, 0.01f) || changed;
    changed = ImGui::DragFloat("Attenuation quadratic", &a->quadratic, 0.01f) || changed;
    return changed;
}


bool ui_directional(render::DirectionalLight* light)
{
    bool changed = false;
    changed = ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    const auto dir_changed = ImGui::DragFloat3("Direction", glm::value_ptr(light->direction), 0.01f);
    if(dir_changed)
    {
        light->direction = glm::normalize(light->direction);
    }
    changed = dir_changed || changed;
    return changed;
}


bool ui_point(render::PointLight* light)
{
    bool changed = false;
    changed = ui_attenuation(&light->attenuation) || changed;
    changed = ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    changed = ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f) || changed;
    return changed;
}


bool ui_spot(render::SpotLight* light)
{
    bool changed = false;
    changed = ui_attenuation(&light->attenuation) || changed;
    changed = ImGui::DragFloat("Ambient strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    changed = ImGui::DragFloat("Cutoff", &light->cutoff, 0.1f) || changed;
    changed = ImGui::DragFloat("Outer cutoff", &light->outer_cutoff, 0.1f) || changed;
    return changed;
}


int
main(int, char**)
{
    const auto pi = 3.14f;

    ///////////////////////////////////////////////////////////////////////////
    // setup
    auto windows = setup();

    if(windows == nullptr)
    {
        LOG_ERROR("Failed to start");
        return 1;
    }

    bool running = true;

#if OLD_INPUT == 1
    constexpr std::string_view quit = "quit";
    constexpr std::string_view leftright = "leftright";
    constexpr std::string_view inout = "inout";
    constexpr std::string_view updown = "updown";
    constexpr std::string_view look_updown = "look_updown";
    constexpr std::string_view look_leftright = "look_leftright";

    auto sdl_input_loaded = input::load(input::config::InputSystem
    {
        {
            {quit, input::Range::within_zero_to_one},
            {leftright, input::Range::within_negative_one_to_positive_one},
            {inout, input::Range::within_negative_one_to_positive_one},
            {updown, input::Range::within_negative_one_to_positive_one},
            {look_updown, input::Range::infinite},
            {look_leftright, input::Range::infinite},
        },

        // controller setup (bind)
        {
            {
                "mouse+keyboard",
                {
                    input::config::KeyboardDefinition{input::KeyboardKey::return_key},
                    input::config::MouseDefinition{}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},
                    input::config::TwoKeyBindDefinition{"leftright", 0, input::KeyboardKey::a, input::KeyboardKey::d},
                    input::config::TwoKeyBindDefinition{"inout", 0, input::KeyboardKey::s, input::KeyboardKey::w},
                    input::config::TwoKeyBindDefinition{"updown", 0, input::KeyboardKey::space, input::KeyboardKey::ctrl_left},

                    // mouse
                    input::config::AxisBindDefinition{"look_leftright", 1, input::Axis2::x, 0.1f},
                    input::config::AxisBindDefinition{"look_updown", 1, input::Axis2::y, 0.1f}
                }
            },
            {
                // probably bad to use a flightstick as a fps controller, but this is the only 'non-gamepad' joystick I own
                // speed link: black widow
                // identified as mega world usb controller
                "joystick",
                {
                    input::config::KeyboardDefinition{},
                    input::config::JoystickDefinition{4, "03000000b50700001703000010010000"}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},

                    // joystick
                    input::config::AxisBindDefinition{"inout",     1, input::AxisType::general_axis, 0, 2},
                    input::config::AxisBindDefinition{"leftright", 1, input::AxisType::hat, 0, 1, 0.5f},
                    input::config::AxisBindDefinition{"updown",    1, input::AxisType::hat, 0, 2, 0.5f},
                    input::config::AxisBindDefinition{"look_leftright", 1, input::AxisType::general_axis, 0, 0, 50.0f, true},
                    input::config::AxisBindDefinition{"look_updown",    1, input::AxisType::general_axis, 0, 1, 50.0f, true}
                }
            },
            {
                "gamecontroller",
                {
                    input::config::KeyboardDefinition{},
                    input::config::GamecontrollerDefinition{}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},

                    // gamecontroller
                    input::config::AxisBindDefinition{"inout",     1, input::GamecontrollerAxis::left_y, 1.0f, true},
                    input::config::AxisBindDefinition{"leftright", 1, input::GamecontrollerAxis::left_x},
                    input::config::TwoKeyBindDefinition{"updown", 1, input::GamecontrollerButton::trigger_left, input::GamecontrollerButton::trigger_right},
                    input::config::AxisBindDefinition{"look_leftright", 1, input::GamecontrollerAxis::right_x, 50.0f},
                    input::config::AxisBindDefinition{"look_updown",    1, input::GamecontrollerAxis::right_y, 50.0f, true}
                }
            },
        }
    });

    if(sdl_input_loaded == false)
    {
        LOG_ERROR("Unable to load input setup: {}", sdl_input_loaded.get_error());
        return -2;
    }

    auto sdl_input = std::move(*sdl_input_loaded.value);
    #endif

    auto sdl_input = DemoInput{true};

    auto camera = render::Camera{};

    auto game_window = windows->add_window("TreD", {1280, 720});
    auto debug_window = windows->add_window("debug", {800, 600});

    auto cube_positions = std::vector<glm::vec3>
    {
        { 0.0f,  0.0f,  0.0f },
        { 2.0f,  5.0f, -5.0f},
        {-1.5f, -2.2f, -2.5f },
        {-3.8f, -2.0f, -5.3f},
        { 2.4f, -0.4f, -3.5f },
        {-1.7f,  3.0f, -7.5f },
        { 1.3f, -2.0f, -2.5f },
        { 1.5f,  2.0f, -2.5f },
        { 1.5f,  0.2f, -1.5f },
        {-1.3f,  1.0f, -1.5f }
    };

    auto vfs = assets::FixedFileVfs{};
    auto engine = render::Engine
    {
        &vfs,
        NUMBER_OF_DIRECTIONAL_LIGHTS,
        NUMBER_OF_POINT_LIGHTS,
        NUMBER_OF_SPOT_LIGHTS
    };

    // todo(Gustav): add override shaders so we can render just white polygon/points
    const auto white_only = engine.add_global_shader(render::MaterialDescription{"unlit.glsl"}); // reuse unlit for white-only shader as it per default is white
    bool use_white_only = false;

    const auto added_crate = engine.add_mesh
    ({
        render::MaterialDescription{"default.glsl"}
            .with_texture(diffuse_texture, "container_diffuse.png")
            .with_texture(specular_texture, "container_specular.png")
            ,
        render::create_box_mesh(1.0f)
    });
    LOG_INFO("Crate is {} {}", added_crate.geom, added_crate.material);

    const auto added_light = engine.add_mesh
    ({
        render::MaterialDescription{"unlit.glsl"},
        render::create_box_mesh(0.2f)
    });
    LOG_INFO("Light is {} {}", added_light.geom, added_light.material);

    const auto added_plane = engine.add_mesh
    ({
        render::MaterialDescription{"default.glsl"}
            .with_texture(diffuse_texture, "container_diffuse.png")
            .with_texture(specular_texture, "container_specular.png")
            ,
        render::create_plane_mesh(plane_size, plane_size)
    });
    LOG_INFO("Plane is {} {}", added_plane.geom, added_plane.material);

    auto get_crate_transform = [&](unsigned int i, float time)
    {
        const auto angle = 20.0f * static_cast<float>(i);
        return glm::rotate
        (
            glm::translate(glm::mat4(1.0f), cube_positions[i]),
            time + glm::radians(angle),
            i%2 == 0
            ? glm::vec3{1.0f, 0.3f, 0.5f}
            : glm::vec3{0.5f, 1.0f, 0.0f}
        );
    };

    auto spot_light = render::SpotLight{};
    auto directional_light = render::DirectionalLight{};

    struct PointLightAndMaterial
    {
        render::PointLight light;
        render::MaterialId material;
        render::PointLightId light_actor;
        render::ActorId mesh_actor;

        PointLightAndMaterial(const glm::vec3& p, render::MaterialId m, render::GeomId light_geom, render::World* world)
            : material(m)
            , light_actor(world->add_point_light(light))
            , mesh_actor(world->add_actor(light_geom, material, glm::translate(glm::mat4(1.0f), light.position)))
        {
            light.position = p;
        }
    };

    auto world = render::create_null_world();

    auto point_lights = std::array<PointLightAndMaterial, NUMBER_OF_POINT_LIGHTS>
    {
        PointLightAndMaterial{glm::vec3{ 0.7f,  0.2f,  2.0f}, added_light.material, added_light.geom, world.get()},
        PointLightAndMaterial{glm::vec3{ 2.3f, -3.3f, -4.0f}, engine.duplicate_material(added_light.material), added_light.geom, world.get()},
        PointLightAndMaterial{glm::vec3{-4.0f,  2.0f, -12.0f}, engine.duplicate_material(added_light.material), added_light.geom, world.get()},
        PointLightAndMaterial{glm::vec3{ 0.0f,  0.0f, -3.0f}, engine.duplicate_material(added_light.material), added_light.geom, world.get()}
    };
    auto match_diffuse_color_for_point_light = [&](PointLightAndMaterial& pl)
    {
        // todo(Gustav): move function to engine?
        auto& material = engine.get_material_ref(pl.material);
        set_vec3_by_lookup(&material, *engine.data, diffuse_color, pl.light.diffuse);
    };
    for(auto& pl: point_lights)
    {
        match_diffuse_color_for_point_light(pl);
    }

    struct Crate
    {
        render::ActorId actor;
        unsigned int id;
        Crate(render::ActorId a, unsigned int i) : actor(a), id(i) {}
    };
    std::vector<Crate> crates;

    // add flying crates
    for(unsigned int i=0; i<cube_positions.size(); i+=1)
    {
        auto actor = world->add_actor(added_crate.geom, added_crate.material, get_crate_transform(i, 0.0f));
        crates.emplace_back(actor, i);
    }

    // add lights
    const auto directional_light_actor = world->add_directional_light(directional_light);
    const auto spot_light_actor = world->add_spot_light(spot_light);
        
    // add ground
    {
        const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
        world->add_actor(added_plane.geom, added_plane.material, model);
    }

    auto cards = render::Texture{::cards::load_texture()};

    float time = 0.0f;
    bool animate = false;
    bool use_custom_hud = false;
    float hud_lerp = 1.0f; // 1= black bars, 0=no black bars

    windows->set_render
    (
        game_window,
        [&](const render::RenderCommand& rc)
        {
            // draw 3d world
            // clear screen
            const auto layout = render::LerpData
            {
                {render::ViewportStyle::extended, 800.0f, 600.0f},
                {render::ViewportStyle::black_bars, 800.0f, 600.0f},
                hud_lerp
            };
            
            // clear to black
            if(hud_lerp > 0.0f)
            {
                rc.clear(glm::vec3{0.0f}, render::LayoutData{render::ViewportStyle::extended, 800.0f, 600.0f});
            }

            // clear background
            rc.clear({0.2f, 0.3f, 0.3f}, layout);

            {
                auto l3 = with_layer3(rc, layout);
                const auto aspect_ratio = get_aspect_ratio(l3.viewport_aabb_in_worldspace);
                
                for(auto& crate: crates)
                {
                    const auto transform = get_crate_transform(crate.id, time);
                    world->update_actor(crate.actor, transform);
                }

                // todo(Gustav): move theese to when light has changed...
                world->update_directional_light(directional_light_actor, directional_light);
                world->update_spot_light(spot_light_actor, spot_light);
                for(auto& pl: point_lights)
                {
                    world->update_point_light(pl.light_actor, pl.light);
                }

                render_world
                (
                    &engine, *world, aspect_ratio, camera,
                    use_white_only ? std::make_optional(white_only) : std::nullopt
                );
            }

            // draw hud
            {
                auto l2 = use_custom_hud
                    ? with_layer2(rc, render::LayoutData{render::ViewportStyle::extended, 800.0f, 600.0f})
                    : with_layer2(rc, layout)
                    ;

                constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(90);

                l2.batch->quad(&cards, card_sprite.translate(10, 10), ::cards::hearts[2]);
            }
        }
    );

    windows->set_render
    (
        debug_window,
        [&](const render::RenderCommand& rc)
        {
            const auto debug_layout = render::LayoutData{render::ViewportStyle::extended, 800.0f, 600.0f};
            rc.clear({0.1f, 0.1f, 0.7f}, debug_layout);
            {
                auto l2 = with_layer2(rc, debug_layout);


                auto tr = [](const glm::vec3& pos) -> Rectf
                {
                    constexpr float half_size = plane_size / 2.0f;
                    constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(90);
                    return card_sprite.translate
                    (
                        remap(-half_size, half_size, pos.x, 0.0f, 800.0f-card_sprite.get_width()),
                        remap(-half_size, half_size, pos.z, 0.0f, 600.0f-card_sprite.get_height())
                    );
                };

                std::size_t i = 0;
                for(const auto p: cube_positions)
                {
                    l2.batch->quad(&cards, tr(p), ::cards::hearts[i]);
                    i = (i+1) % ::cards::hearts.size();
                }
                l2.batch->quad(&cards, tr(camera.position), ::cards::back);
            }
        }
    );

    int rendering_mode = 0;
    const auto set_rendering_mode = [&rendering_mode, &use_white_only]()
    {
        switch(rendering_mode)
        {
            case 0: render::set_poly_mode_to_fill(); use_white_only = false; break;
            case 1: render::set_poly_mode_to_line(); use_white_only = true; break;
            case 2: render::set_poly_mode_to_point(); use_white_only = true; break;
            default: DIE("invalid rendering_mode"); break;
        }
    };

    windows->set_imgui
    (
        [&]()
        {

            if(ImGui::Begin("Debug"))
            {
                if(ImGui::Button("Quit")) { running = false; }
                ImGui::SameLine();
                ImGui::Checkbox("Animate?", &animate);
                ImGui::SameLine();
                ImGui::SliderFloat("Time", &time, 0.0f, 2 * pi);
                
                ImGui::SliderFloat("Black bars?", &hud_lerp, 0.0f, 1.0f);
                ImGui::SameLine();
                ImGui::Checkbox("Custom hud?", &use_custom_hud);

                if(ImGui::Combo("Rendering mode", &rendering_mode, "Fill\0Line\0Point\0"))
                {
                    set_rendering_mode();
                }
                ImGui::DragFloat("FOV", &camera.fov, 0.1f, 1.0f, 145.0f);

                if (ImGui::CollapsingHeader("Lights"))
                {
                    if(ImGui::CollapsingHeader("Directional"))
                    {
                        ImGui::PushID("directional light");
                        ui_directional(&directional_light);
                        ImGui::PopID();
                    }
                    if(ImGui::CollapsingHeader("Spot"))
                    {
                        ImGui::PushID("spot light");
                        ui_spot(&spot_light);
                        ImGui::PopID();
                    }
                    if(ImGui::CollapsingHeader("Point"))
                    {
                        for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                        {
                            ImGui::PushID(static_cast<int>(i));
                            if(ui_point(&point_lights[i].light))
                            {
                                match_diffuse_color_for_point_light(point_lights[i]);
                            }
                            ImGui::PopID();
                        }
                    }
                }

                if (ImGui::CollapsingHeader("Cubes"))
                {
                    // todo(Gustav): introduce again
                    #if 0
                    ImGui::ColorEdit4("Cube colors", glm::value_ptr(cube_color));
                    ImGui::ColorEdit3("Tint color", glm::value_ptr(material.tint));
                    ImGui::DragFloat("Specular strength", &material.specular_strength, 0.01f);
                    ImGui::DragFloat("Shininess", &material.shininess, 1.0f, 2.0f, 256.0f);
                    #endif
                    for(auto& cube: cube_positions)
                    {
                        ImGui::PushID(&cube);
                        ImGui::DragFloat3("", glm::value_ptr(cube), 0.01f);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::End();
        }
    );

    #if OLD_INPUT == 1
    auto player = sdl_input.add_player();
    auto get = [](const input::Table& table, std::string_view name, float d=0.0f) -> float
    {
        const auto found = table.data.find(std::string{name});
        if(found == table.data.end()) { return d; }
        return found->second;
    };
    #endif

    return main_loop(input::unit_discovery::find_highest, std::move(windows), &sdl_input, [&](float dt) -> bool
    {
        #if OLD_INPUT == 1
        input::Table table;
        sdl_input.update_table(player, &table, dt);
        #endif

        if(animate)
        {
            time += dt;

            if(time > 2*pi)
            {
                time -= 2*pi;
            }
        }

        const bool should_quit
        #if OLD_INPUT == 1
        = get(table, quit, 1.0f) > 0.5f
        #else
        = sdl_input.should_quit
        #endif
        ;

        if(should_quit)
        {
            running = false;
        }

        const auto v = create_vectors(camera);

        #if OLD_INPUT == 1
        const auto input_inout = get(table, inout);
        const auto input_leftright = get(table, leftright);
        const auto input_updown = get(table, updown);
        const auto input_look_leftright = get(table, look_leftright);
        const auto input_look_updown = get(table, look_updown);
        #else
        const auto input_inout = sdl_input.get_inout();
        const auto input_leftright = sdl_input.get_leftright();
        const auto input_updown = sdl_input.get_updown();
        const auto input_look_leftright = sdl_input.get_look_leftright();
        const auto input_look_updown = sdl_input.get_look_updown();
        #endif

        const auto camera_movement
            = v.front * input_inout
            + v.right * input_leftright
            + v.up * input_updown
            ;

        constexpr bool input_shift = false; // todo(Gustav): expose toggleables
        const auto camera_speed = 3 * dt * (input_shift ? 2.0f : 1.0f);
        camera.position += camera_speed * camera_movement;

        // LOG_INFO("mouse: {} {}", get(table, look_leftright), get(table, look_updown));
        const float sensitivity = 1.0f;
        camera.yaw += input_look_leftright * sensitivity;
        camera.pitch += input_look_updown * sensitivity;
        if(camera.pitch >  89.0f) camera.pitch =  89.0f;
        if(camera.pitch < -89.0f) camera.pitch = -89.0f;

        return running;
    });
}

