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
#include "tred/texture.h"

SpriteBatch::SpriteBatch(Shader* quad_shader, Render2* r)
    : render(r)
{
    quad_shader->Use();

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

void add_vertex(SpriteBatch* batch, const vertex2& v)
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

void SpriteBatch::quad(Texture* texture, const vertex2& v0, const vertex2& v1, const vertex2& v2, const vertex2& v3)
{
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

void SpriteBatch::quad(Texture* texture, const rect& scr, const rect& tex, const glm::vec4& tint)
{
    quad
    (
        texture,
        {{scr.minx, scr.miny, 0.0f}, tint, {tex.minx, tex.miny}},
        {{scr.maxx, scr.miny, 0.0f}, tint, {tex.maxx, tex.miny}},
        {{scr.maxx, scr.maxy, 0.0f}, tint, {tex.maxx, tex.maxy}},
        {{scr.minx, scr.maxy, 0.0f}, tint, {tex.minx, tex.maxy}}
    );
}

void SpriteBatch::submit()
{
    if(quads == 0)
    {
        return;
    }

    BindTexture(render->texture_uniform, *current_texture);
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


Render2::Render2()
    : quad_description
    (
        {
            {VertexType::Position2, "position"},
            {VertexType::Color4, "color"},
            {VertexType::Texture2, "uv"}
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
    , view_projection_uniform(quad_shader.GetUniform("view_projection"))
    , transform_uniform(quad_shader.GetUniform("transform"))
    , texture_uniform(quad_shader.GetUniform("uniform_texture"))
    , batch(&quad_shader, this)
{
    SetupTextures(&quad_shader, {&texture_uniform});
}

Render2::~Render2()
{
    quad_shader.Delete();
}


void Game::OnRender(const RenderCommand2&) {}
void Game::OnImgui() {}
bool Game::OnUpdate(float) { return true; }
void Game::OnKey(char, bool) {}
void Game::OnMouseMotion(const glm::ivec2&) {}
void Game::OnMouseButton(int, bool) {}
void Game::OnMouseWheel(int) {}


void SetupOpenGl(SDL_Window* window, SDL_GLContext glcontext, bool imgui)
{
    SetupOpenglDebug();
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


struct Window
{
    std::string title;
    glm::ivec2 size;
    bool imgui;
    bool running = true;

    SDL_Window* window;
    SDL_GLContext glcontext;

    std::shared_ptr<Game> game;
    std::unique_ptr<Render2> render_data;

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

        glViewport(0, 0, size.x, size.y);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        game->OnRender({render_data.get(), size});

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
    window.render_data = std::make_unique<Render2>();
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

