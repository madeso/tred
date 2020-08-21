#include "glad/glad.h"
#include "SDL.h"

#include <string_view>
#include <cassert>

#include "debug_opengl.h"

#include "vertex.glsl.h"
#include "fragment.glsl.h"


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}



int
main(int, char**)
{
    ///////////////////////////////////////////////////////////////////////////
    // setup
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
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

    int width = 1280;
    int height = 720;

    SDL_Window* window = SDL_CreateWindow
    (
        "TreD",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if(window == nullptr)
    {
        SDL_Log("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize OpenGL context");
        return -1;
    }

    SetupOpenglDebug();

    const auto* renderer = glGetString(GL_RENDERER); // get renderer string
    const auto* version = glGetString(GL_VERSION); // version as a string
    SDL_Log("Renderer: %s\n", renderer);
    SDL_Log("Version: %s\n", version);

    const auto update_viewport = [&]()
    {
        glViewport(0, 0, width, height);
    };

    update_viewport();

    int rendering_mode = 0;
    const auto set_rendering_mode = [&rendering_mode](int new_mode)
    {
        if(rendering_mode == new_mode)
        {
            return;
        }

        rendering_mode = new_mode;
        switch(rendering_mode)
        {
            case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
            case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
            default: assert(false && "invalid rendering_mode"); break;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // shaders
    const auto check_shader_compilation_error = [] (const char* name, unsigned int shader)
    {
        int  success = 0;
        char log[512] = {0,};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, log);
            SDL_Log("ERROR: %s shader compilation failed\n%s\n", name, log);
        }
    };

    const auto check_shader_link_error = [](unsigned int program)
    {
        int  success = 0;
        char log[512] = {0,};
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(program, 512, NULL, log);
            SDL_Log("ERROR: shader linking failed\n%s\n", log);
        }
    };

    const auto upload_shader_source =
        [](unsigned int shader, std::string_view source)
    {
        const char* const s = &source[0];
        const int length = Csizet_to_int(source.length());
        glShaderSource(shader, 1, &s, &length);
    };

    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    upload_shader_source(vertex_shader, VERTEX_GLSL);
    glCompileShader(vertex_shader);
    check_shader_compilation_error("vertex", vertex_shader);

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    upload_shader_source(fragment_shader, FRAGMENT_GLSL);
    glCompileShader(fragment_shader);
    check_shader_compilation_error("fragment", fragment_shader);

    const auto shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    check_shader_link_error(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(0);

    ///////////////////////////////////////////////////////////////////////////
    // model
    constexpr float vertices[] =
    {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    constexpr unsigned int indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    const auto generate_buffer = []()
    {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        return buffer;
    };

    const auto generate_vertex_array = []()
    {
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        return vao;
    };

    // defined in glsl
    const auto vertex_position_location = 0;

    const auto vbo = generate_buffer();
    const auto vao = generate_vertex_array();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_position_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // class: use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = generate_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ///////////////////////////////////////////////////////////////////////////
    // main

    bool running = true;

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type)
            {
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    width = e.window.data1;
                    height = e.window.data2;
                    update_viewport();
                }
                break;
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                const bool down = e.type == SDL_KEYDOWN;

                switch(e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    if(down)
                    {
                        running = false;
                    }
                    break;
                case SDLK_TAB:
                    if(down)
                    {
                        set_rendering_mode((rendering_mode + 1) % 3);
                    }
                    break;
                default:
                    // ignore other keys
                    break;
                }
            }
            break;
            default:
                // ignore other events
                break;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);

    glUseProgram(0);
    glDeleteProgram(shader_program);

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

