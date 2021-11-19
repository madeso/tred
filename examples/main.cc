// standard headers
#include <string>
#include <string_view>
#include <cassert>
#include <vector>
#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>

#include "tred/compiler_warning.h"

#include <fmt/core.h>

// dependency headers
#include "glad/glad.h"
#include "tred/dependency_glm.h"

#include "tred/undef_windows.h"

// imgui
#include "tred/dependency_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "tred/dependency_sdl.h"
#include "tred/opengl_debug.h"
#include "tred/cint.h"
#include "tred/mesh.h"
#include "tred/mesh.default.h"
#include "tred/texture.h"
#include "tred/uniform.h"
#include "tred/shader.h"
#include "tred/log.h"
#include "tred/mesh.compiled.h"
#include "tred/texture.load.h"
#include "tred/windows.h"
#include "tred/input/system.h"
#include "tred/input/config.h"
#include "tred/input/table.h"
#include "tred/layer2.h"
#include "tred/spritebatch.h"

// resource headers
#include "shader_vertex.glsl.h"
#include "shader_fragment.glsl.h"
#include "light_vertex.glsl.h"
#include "light_fragment.glsl.h"
#include "container_diffuse.png.h"
#include "container_specular.png.h"

#include "sprites/cards.h"


struct Material
{
    Texture diffuse;
    Texture specular;

    glm::vec3 tint = glm::vec3{1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;

    float specular_strength = 1.0f;

    explicit Material(Texture&& d, Texture&& s)
        : diffuse(std::move(d))
        , specular(std::move(s))
    {
    }
};


struct MaterialUniforms
{
    Uniform diffuse;
    Uniform specular;

    Uniform tint;
    Uniform shininess;
    Uniform specular_strength;

    MaterialUniforms
    (
        ShaderProgram* shader,
        const std::string& base_name
    )
        : diffuse(shader->get_uniform(base_name + ".diffuse"))
        , specular(shader->get_uniform(base_name + ".specular"))
        , tint(shader->get_uniform(base_name + ".tint"))
        , shininess(shader->get_uniform(base_name + ".shininess"))
        , specular_strength(shader->get_uniform(base_name + ".specular_strength" ))
    {
        setup_textures(shader, {&diffuse, &specular});
    }

    void
    set_shader(ShaderProgram* shader, const Material& material) const
    {
        bind_texture(diffuse, material.diffuse);
        bind_texture(specular, material.specular);

        shader->set_vec3(tint, material.tint);
        shader->set_float(specular_strength, material.specular_strength);
        shader->set_float(shininess, material.shininess);
    }
};


struct DirectionalLight
{
    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};

    // todo(Gustav): make a property instead, function only here for demo purposes
    glm::vec3
    GetDirection() const
    {
        return glm::normalize(-position);
    }
};


struct DirectionalLightUniforms
{
    Uniform direction;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    DirectionalLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        direction(shader.get_uniform(base_name + ".normalized_direction")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(ShaderProgram* shader, const DirectionalLight& light) const
    {
        shader->set_vec3(direction, light.GetDirection());
        shader->set_vec3(ambient, light.ambient * light.ambient_strength);
        shader->set_vec3(diffuse, light.diffuse);
        shader->set_vec3(specular, light.specular);
    }
};


struct Attenuation
{
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};


struct AttenuationUniforms
{
    Uniform constant;
    Uniform linear;
    Uniform quadratic;

    AttenuationUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        constant(shader.get_uniform(base_name + ".constant")),
        linear(shader.get_uniform(base_name + ".linear")),
        quadratic(shader.get_uniform(base_name + ".quadratic"))
    {
    }

    void
    set_shader(ShaderProgram* shader, const Attenuation& att) const
    {
        shader->set_float(constant, att.constant);
        shader->set_float(linear, att.linear);
        shader->set_float(quadratic, att.quadratic);
    }
};


struct PointLight
{
    Attenuation attenuation;

    glm::vec3 position;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};

    PointLight(const glm::vec3& p) : position(p) {}
};


struct PointLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    PointLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.get_uniform(base_name + ".position")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(ShaderProgram* shader, const PointLight& light) const
    {
        attenuation.set_shader(shader, light.attenuation);
        shader->set_vec3(position, light.position);
        shader->set_vec3(ambient, light.ambient * light.ambient_strength);
        shader->set_vec3(diffuse, light.diffuse);
        shader->set_vec3(specular, light.specular);
    }
};


struct SpotLight
{
    Attenuation attenuation;

    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 direction = glm::vec3{0.0f, 0.0f, 0.0f};
    float cutoff = 12.5f;
    float outer_cutoff = 17.5;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};
};


struct SpotLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform direction;
    Uniform cos_cutoff;
    Uniform cos_outer_cutoff;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    SpotLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.get_uniform(base_name + ".position")),
        direction(shader.get_uniform(base_name + ".direction")),
        cos_cutoff(shader.get_uniform(base_name + ".cos_cutoff")),
        cos_outer_cutoff(shader.get_uniform(base_name + ".cos_outer_cutoff")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(ShaderProgram* shader, const SpotLight& light) const
    {
        attenuation.set_shader(shader, light.attenuation);
        shader->set_vec3(position, light.position);
        shader->set_vec3(direction, light.direction);
        shader->set_float(cos_cutoff, cos(glm::radians(light.cutoff)));
        shader->set_float(cos_outer_cutoff, cos(glm::radians(light.outer_cutoff)));
        shader->set_vec3(ambient, light.ambient * light.ambient_strength);
        shader->set_vec3(diffuse, light.diffuse);
        shader->set_vec3(specular, light.specular);
    }
};


constexpr unsigned int NUMBER_OF_POINT_LIGHTS = 4;


constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);


struct CameraVectors
{
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 position;

    CameraVectors(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& p)
        : front(f)
        , right(r)
        , up(u)
        , position(p)
    {
    }
};


struct Camera
{
    float fov = 45.0f;

    float near = 0.1f;
    float far = 100.0f;

    glm::vec3 position = glm::vec3{0.0f, 0.0f,  3.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    CameraVectors create_vectors() const
    {
        const auto direction = glm::vec3
        {
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        };
        const auto front = glm::normalize(direction);
        const auto right = glm::normalize(glm::cross(front, UP));
        const auto up = glm::normalize(glm::cross(right, front));

        return {front, right, up, position};
    }
};


struct CompiledCamera
{
    glm::mat4 view;
    glm::vec3 position;

    CompiledCamera(const glm::mat4& v, const glm::vec3& p)
        : view(v)
        , position(p)
    {
    }
};


CompiledCamera compile(const CameraVectors& camera)
{
    const auto view = glm::lookAt(camera.position, camera.position + camera.front, UP);
    return {view, camera.position};
}

float get_aspect_ratio(const Rectf& r)
{
    return r.get_width() / r.get_height();
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

    auto camera = ::Camera{};

    auto game_window = windows->add_window("TreD", {1280, 720});
    auto debug_window = windows->add_window("debug", {800, 600});

    ///////////////////////////////////////////////////////////////////////////
    // shader layout
    const auto mesh_layout = VertexLayoutDescription
    {
        {VertexType::position3, "aPos"},
        {VertexType::normal3, "aNormal"},
        {VertexType::color4, "aColor"},
        {VertexType::texture2, "aTexCoord"}
    };
    auto layout_compiler = compile({mesh_layout});
    const auto compiled_layout = layout_compiler.compile(mesh_layout);

    const auto light_layout = VertexLayoutDescription
    {
        {VertexType::position3, "aPos"}
    };
    auto light_compiler = compile({light_layout});
    const auto compiled_light_layout = light_compiler.compile(light_layout);

    ///////////////////////////////////////////////////////////////////////////
    // shaders
    auto shader = ::ShaderProgram{SHADER_VERTEX_GLSL, SHADER_FRAGMENT_GLSL, compiled_layout};
    const auto uni_color = shader.get_uniform("uColor");
    const auto uni_transform = shader.get_uniform("uTransform");
    const auto uni_model_transform = shader.get_uniform("uModelTransform");
    const auto uni_normal_matrix = shader.get_uniform("uNormalMatrix");
    const auto uni_view_position = shader.get_uniform("uViewPosition");
    const auto uni_material = MaterialUniforms{&shader, "uMaterial"};
    const auto uni_directional_light = DirectionalLightUniforms{shader, "uDirectionalLight"};
    const auto uni_point_lights = std::array<PointLightUniforms, NUMBER_OF_POINT_LIGHTS>
    {
        PointLightUniforms{shader, "uPointLights[0]"},
        PointLightUniforms{shader, "uPointLights[1]"},
        PointLightUniforms{shader, "uPointLights[2]"},
        PointLightUniforms{shader, "uPointLights[3]"}
    };
    const auto uni_spot_light = SpotLightUniforms{shader, "uSpotLight"};

    auto light_shader = ::ShaderProgram{LIGHT_VERTEX_GLSL, LIGHT_FRAGMENT_GLSL, compiled_light_layout};
    const auto uni_light_transform = light_shader.get_uniform("uTransform");
    const auto uni_light_color = light_shader.get_uniform("uColor");

    ///////////////////////////////////////////////////////////////////////////
    // model
    const auto mesh = compile(create_box_mesh(1.0f), compiled_layout);
    const auto light_mesh = compile(create_box_mesh(0.2f), compiled_light_layout);
    const auto plane_mesh = compile(create_plane_mesh(20.0f, 20.0f), compiled_layout);

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
    auto cube_color = glm::vec4{1.0f};

    auto material = ::Material
    {
        load_image_from_embedded
        (
            CONTAINER_DIFFUSE_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        ),
        load_image_from_embedded
        (
            CONTAINER_SPECULAR_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        )
    };
    auto directional_light = ::DirectionalLight{};
    auto point_lights = std::array<PointLight, NUMBER_OF_POINT_LIGHTS>
    {
        glm::vec3{ 0.7f,  0.2f,  2.0f},
        glm::vec3{ 2.3f, -3.3f, -4.0f},
        glm::vec3{-4.0f,  2.0f, -12.0f},
        glm::vec3{ 0.0f,  0.0f, -3.0f}
    };
    auto spot_light = ::SpotLight{};

    auto cards = Texture{::cards::load_texture()};

    float time = 0.0f;
    bool animate = false;
    bool use_custom_hud = false;
    float hud_lerp = 1.0f; // 1= black bars, 0=no black bars

    windows->set_render
    (
        game_window,
        [&](const RenderCommand& rc)
        {
            // draw 3d world
            // clear screen
            const auto layout = LerpData
            {
                {ViewportStyle::extended, 800.0f, 600.0f},
                {ViewportStyle::black_bars, 800.0f, 600.0f},
                hud_lerp
            };
            
            // clear to black
            if(hud_lerp > 0.0f)
            {
                rc.clear(glm::vec3{0.0f}, LayoutData{ViewportStyle::extended, 800.0f, 600.0f});
            }

            // clear background
            rc.clear({0.2f, 0.3f, 0.3f}, layout);

            {
                auto l3 = with_layer3(rc, layout);

                const auto aspect_ratio = get_aspect_ratio(l3.viewport_aabb_in_worldspace);
                const glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near, camera.far);

                // todo(Gustav): move clear to rc
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                const auto compiled_camera = compile(camera.create_vectors());
            
                const auto view = compiled_camera.view;

                const auto pv = projection * view;

                for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                {
                    light_shader.use();
                    light_shader.set_vec3(uni_light_color, point_lights[i].diffuse);
                    {
                        const auto model = glm::translate(glm::mat4(1.0f), point_lights[i].position);
                        light_shader.set_mat(uni_light_transform, pv * model);
                    }
                    light_mesh.draw();
                }

                shader.use();
                shader.set_vec4(uni_color, cube_color);
                uni_material.set_shader(&shader, material);
                uni_directional_light.set_shader(&shader, directional_light);
                uni_spot_light.set_shader(&shader, spot_light);
                for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                {
                    uni_point_lights[i].set_shader(&shader, point_lights[i]);
                }
                shader.set_vec3(uni_view_position, compiled_camera.position);

                for(unsigned int i=0; i<cube_positions.size(); i+=1)
                {
                    const auto angle = 20.0f * static_cast<float>(i);
                    {
                        const auto model = glm::rotate
                        (
                            glm::translate(glm::mat4(1.0f), cube_positions[i]),
                            time + glm::radians(angle),
                            i%2 == 0
                            ? glm::vec3{1.0f, 0.3f, 0.5f}
                            : glm::vec3{0.5f, 1.0f, 0.0f}
                        );
                        shader.set_mat(uni_transform, pv * model);
                        shader.set_mat(uni_model_transform, model);
                        shader.set_mat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                    }
                    mesh.draw();
                }

                {
                    const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
                    shader.set_mat(uni_transform, pv * model);
                    shader.set_mat(uni_model_transform, model);
                    shader.set_mat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                    plane_mesh.draw();
                }
            }

            // draw hud
            {
                auto l2 = use_custom_hud
                    ? with_layer2(rc, LayoutData{ViewportStyle::extended, 800.0f, 600.0f})
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
        [&](const RenderCommand& rc)
        {
            const auto debug_layout = LayoutData{ViewportStyle::extended, 800.0f, 600.0f};
            rc.clear({0.1f, 0.1f, 0.7f}, debug_layout);
            {
                auto l2 = with_layer2(rc, debug_layout);

                constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(90);
                l2.batch->quad(&cards, card_sprite.translate(10, 10), ::cards::hearts[1]);
                l2.batch->quad(&cards, card_sprite.translate(30, 10), ::cards::hearts[2]);
            }
        }
    );

    int rendering_mode = 0;
    const auto set_rendering_mode = [&rendering_mode]()
    {
        switch(rendering_mode)
        {
            case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
            case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
            default: assert(false && "invalid rendering_mode"); break;
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
                    const auto ui_attenuation = [](Attenuation* a)
                    {
                        ImGui::DragFloat("Attenuation constant", &a->constant, 0.01f);
                        ImGui::DragFloat("Attenuation linear", &a->linear, 0.01f);
                        ImGui::DragFloat("Attenuation quadratic", &a->quadratic, 0.01f);
                    };

                    const auto ui_directional = [](DirectionalLight* light)
                    {
                        ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));
                        ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f);
                    };

                    const auto ui_point = [&ui_attenuation](PointLight* light)
                    {
                        ui_attenuation(&light->attenuation);
                        ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));
                        ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f);
                    };

                    const auto ui_spot = [&ui_attenuation](SpotLight* light)
                    {
                        ui_attenuation(&light->attenuation);

                        ImGui::DragFloat("Ambient strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));

                        ImGui::DragFloat("Cutoff", &light->cutoff, 0.1f);
                        ImGui::DragFloat("Outer cutoff", &light->outer_cutoff, 0.1f);
                    };

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
                            ui_point(&point_lights[i]);
                            ImGui::PopID();
                        }
                    }
                }

                if (ImGui::CollapsingHeader("Cubes"))
                {
                    ImGui::ColorEdit4("Cube colors", glm::value_ptr(cube_color));
                    ImGui::ColorEdit3("Tint color", glm::value_ptr(material.tint));
                    ImGui::DragFloat("Specular strength", &material.specular_strength, 0.01f);
                    ImGui::DragFloat("Shininess", &material.shininess, 1.0f, 2.0f, 256.0f);
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

    auto player = sdl_input.add_player();
    auto get = [](const input::Table& table, std::string_view name, float d=0.0f) -> float
    {
        const auto found = table.data.find(std::string{name});
        if(found == table.data.end()) { return d; }
        return found->second;
    };

    return main_loop(input::unit_discovery::find_highest, std::move(windows), &sdl_input, [&](float dt) -> bool
    {
        input::Table table;
        sdl_input.update_table(player, &table, dt);

        if(animate)
        {
            time += dt;

            if(time > 2*pi)
            {
                time -= 2* pi;
            }
        }

        if(get(table, quit, 1.0f) > 0.5f)
        {
            running = false;
        }

        const auto v = camera.create_vectors();

        const auto input_inout = get(table, inout);
        const auto input_leftright = get(table, leftright);
        const auto input_updown = get(table, updown);

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
        camera.yaw += get(table, look_leftright) * sensitivity;
        camera.pitch += get(table, look_updown) * sensitivity;
        if(camera.pitch >  89.0f) camera.pitch =  89.0f;
        if(camera.pitch < -89.0f) camera.pitch = -89.0f;

        // constexpr std::string_view quit = "quit";
        return running;
    });
}

