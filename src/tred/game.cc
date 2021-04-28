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

#include "glm/gtc/matrix_transform.hpp"

#include "tred/log.h"
#include "tred/opengl.debug.h"
#include "tred/types.h"
#include "tred/image.h"
#include "tred/viewportdef.h"

#include "tred/windows.sdl.convert.h"

void set_gl_viewport(const recti& r)
{
    glViewport(r.minx, r.miny, r.get_width(), r.get_height());
}

render_layer2::~render_layer2()
{
    batch->submit();
}

render_layer2::render_layer2(layer2&& l, sprite_batch* b)
    : layer2(l)
    , batch(b)
{
}

layer2 create_layer(const viewport_definition& vp)
{
    return {{vp.virtual_width, vp.virtual_height}, vp.screen_rect};
}

render_layer2 create_layer(const render_command2& rc, const viewport_definition& vp, const glm::mat4 camera)
{
    set_gl_viewport(vp.screen_rect);

    const auto projection = glm::ortho(0.0f, vp.virtual_width, 0.0f, vp.virtual_height);

    rc.render->quad_shader.use();
    rc.render->quad_shader.set_mat(rc.render->view_projection_uniform, projection);
    rc.render->quad_shader.set_mat(rc.render->transform_uniform, camera);

    // todo(Gustav): transform viewport according to the camera
    return render_layer2{create_layer(vp), &rc.render->batch};
}

glm::vec2 layer2::mouse_to_world(const glm::vec2& p) const
{
    // transform from mouse pixels to window 0-1
    const auto n = screen.cast<float>().to01(p);
    return viewport_aabb_in_worldspace.from01(n);
}

render_layer2 with_layer_fit_with_bars(const render_command2& rc, float requested_width, float requested_height, const glm::mat4 camera)
{
    const auto vp = viewport_definition::fit_with_black_bars(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(rc, vp, camera);
}

render_layer2 with_layer_extended(const render_command2& rc, float requested_width, float requested_height, const glm::mat4 camera)
{
    const auto vp = viewport_definition::extend(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(rc, vp, camera);
}

layer2 with_layer_fit_with_bars(const command2& rc, float requested_width, float requested_height, const glm::mat4)
{
    const auto vp = viewport_definition::fit_with_black_bars(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(vp);
}

layer2 with_layer_extended(const command2& rc, float requested_width, float requested_height, const glm::mat4)
{
    const auto vp = viewport_definition::extend(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(vp);
}

sprite_batch::sprite_batch(shader* quad_shader, render2* r)
    : render(r)
    , white_texture
    (
        load_image_from_color
        (
            0xffffffff,
            texture_edge::clamp,
            texture_render_style::pixel,
            transparency::include
        )
    )
{
    quad_shader->use();

    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    constexpr auto vertex_size = 9 * sizeof(float);
    constexpr auto max_vertices = 4 * max_quads;
    constexpr auto max_indices = 6 * max_quads;

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(7 * sizeof(float)));

    std::vector<u32> indices;
    indices.reserve(max_indices);

    for(auto quad_index=0; quad_index<max_quads; quad_index+=1)
    {
        const auto base = quad_index * 4;
        indices.emplace_back(base + 0); indices.emplace_back(base + 1); indices.emplace_back(base + 2);
        indices.emplace_back(base + 2); indices.emplace_back(base + 3); indices.emplace_back(base + 0);
    }

    assert(max_indices == indices.size());

    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW);
}

void add_vertex(sprite_batch* batch, const vertex2& v)
{
    batch->data.push_back(v.position.x);
    batch->data.push_back(v.position.y);
    batch->data.push_back(v.position.z);

    batch->data.push_back(v.color.x);
    batch->data.push_back(v.color.y);
    batch->data.push_back(v.color.z);
    batch->data.push_back(v.color.w);

    batch->data.push_back(v.texturecoord.x);
    batch->data.push_back(v.texturecoord.y);
}

rect get_sprite(const texture& texture, const recti& ri)
{
    const auto r = Cint_to_float(ri);
    const auto w = 1.0f/static_cast<float>(texture.width);
    const auto h = 1.0f/static_cast<float>(texture.height);
    return {r.minx * w, 1-r.maxy * h, r.maxx * w, 1-r.miny * h};
}

void sprite_batch::quad(std::optional<texture*> texture_argument, const vertex2& v0, const vertex2& v1, const vertex2& v2, const vertex2& v3)
{
    texture* texture = texture_argument.value_or(&white_texture);

    if(quads == max_quads)
    {
        submit();
    }

    if(current_texture == nullptr)
    {
        current_texture = texture;
    }
    else if (current_texture != texture)
    {
        submit();
        current_texture = texture;
    }

    quads += 1;

    add_vertex(this, v0);
    add_vertex(this, v1);
    add_vertex(this, v2);
    add_vertex(this, v3);
}

void sprite_batch::quad(std::optional<texture*> texture, const rect& scr, const std::optional<rect>& texturecoord, const glm::vec4& tint)
{
    const auto tc = texturecoord.value_or(rect{1.0f, 1.0f});
    quad
    (
        texture,
        {{scr.minx, scr.miny, 0.0f}, tint, {tc.minx, tc.miny}},
        {{scr.maxx, scr.miny, 0.0f}, tint, {tc.maxx, tc.miny}},
        {{scr.maxx, scr.maxy, 0.0f}, tint, {tc.maxx, tc.maxy}},
        {{scr.minx, scr.maxy, 0.0f}, tint, {tc.minx, tc.maxy}}
    );
}

void sprite_batch::quad(std::optional<texture*> texture_argument, const rect& scr, const recti& texturecoord, const glm::vec4& tint)
{
    texture* texture = texture_argument.value_or(&white_texture);
    quad(texture, scr, get_sprite(*texture, texturecoord), tint);
}

void sprite_batch::submit()
{
    if(quads == 0)
    {
        return;
    }

    bind_texture(render->texture_uniform, *current_texture);
    glBindVertexArray(va);
    glBufferSubData
    (
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(sizeof(float) * data.size()),
        static_cast<const void*>(data.data())
    );
    glDrawElements(GL_TRIANGLES, 6 * quads, GL_UNSIGNED_INT, nullptr);

    data.resize(0);
    quads = 0;
    current_texture = nullptr;
}


render2::render2()
    : quad_description
    (
        {
            {vertex_type::position2, "position"},
            {vertex_type::color4, "color"},
            {vertex_type::texture2, "uv"}
        }
    )
    , quad_layout
    (
        compile({quad_description}).compile(quad_description)
    )
    , quad_shader
    (
        R"glsl(
            #version 450 core
            in vec3 position;
            in vec4 color;
            in vec2 uv;

            uniform mat4 view_projection;
            uniform mat4 transform;

            out vec4 varying_color;
            out vec2 varying_uv;

            void main()
            {
                varying_color = color;
                varying_uv = uv;
                gl_Position = view_projection * transform * vec4(position, 1.0);
            }
        )glsl",
        R"glsl(
            #version 450 core

            in vec4 varying_color;
            in vec2 varying_uv;

            uniform sampler2D uniform_texture;

            out vec4 color;

            void main()
            {
                color = texture(uniform_texture, varying_uv) * varying_color;
            }
        )glsl",
        quad_layout
    )
    , view_projection_uniform(quad_shader.get_uniform("view_projection"))
    , transform_uniform(quad_shader.get_uniform("transform"))
    , texture_uniform(quad_shader.get_uniform("uniform_texture"))
    , batch(&quad_shader, this)
{
    setup_textures(&quad_shader, {&texture_uniform});
}

render2::~render2()
{
    quad_shader.cleanup();
}


void game::on_render(const render_command2&) {}
void game::on_imgui() {}
bool game::on_update(float) { return true; }
void game::on_key(char, bool) {}
void game::on_mouse_position(const glm::ivec2&) {}
void game::on_mouse_button(const command2&, input::mouse_button, bool) {}
void game::on_mouse_wheel(int) {}

namespace
{
    void setup_open_gl(SDL_Window* window, SDL_GLContext glcontext, bool imgui)
    {
        setup_opengl_debug();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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


struct window
{
    std::string title;
    glm::ivec2 size;
    bool imgui;
    bool running = true;

    SDL_Window* sdl_window;
    SDL_GLContext sdl_glcontext;

    std::shared_ptr<game> game;
    std::unique_ptr<render2> render_data;

    window(const std::string& t, const glm::ivec2& s, bool i)
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

    ~window()
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


void pump_events(window* window)
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
                window->game->on_mouse_position(glm::ivec2{e.motion.x, window->size.y - e.motion.y});
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


int setup_and_run(std::function<std::shared_ptr<game>()> make_game, const std::string& title, const glm::ivec2& size, bool call_imgui)
{
    auto window = ::window{title, size, call_imgui};
    if(window.sdl_window == nullptr)
    {
        return -1;
    }
    window.render_data = std::make_unique<render2>();
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


int run_game(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<game>()> make_game)
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

