#include "glad/glad.h"
#include "SDL.h"

#include <string>
#include <string_view>
#include <cassert>

#include "debug_opengl.h"

#include "vertex.glsl.h"
#include "fragment.glsl.h"

#include "stb_image.h"


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}

unsigned int CreateTexture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}

unsigned int
LoadImage(unsigned char* image_source, int size)
{
    const auto texture = CreateTexture();
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0;
    int height = 0;
    int junk_channels;
    auto* pixel_data = stbi_load_from_memory
    (
        image_source, size,
        &width, &height,
        &junk_channels, 3
    );

    if(pixel_data != nullptr)
    {
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width, height,
            0,
            GL_RGB, GL_UNSIGNED_BYTE,
            pixel_data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(pixel_data);
    }

    return texture;
}

bool
CheckShaderCompilationError(const char* name, unsigned int shader)
{
    int  success = 0;
    char log[512] = {0,};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        SDL_Log("ERROR: %s shader compilation failed\n%s\n", name, log);
        return false;
    }

    return true;
}


bool
CheckShaderLinkError(unsigned int program)
{
    int  success = 0;
    char log[512] = {0,};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        SDL_Log("ERROR: shader linking failed\n%s\n", log);
        return false;
    }

    return true;
}


void
UploadShaderSource(unsigned int shader, std::string_view source)
{
    const char* const s = &source[0];
    const int length = Csizet_to_int(source.length());
    glShaderSource(shader, 1, &s, &length);
};


unsigned int CreateBuffer()
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    return buffer;
};


unsigned int CreateVertexArray()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    return vao;
};


struct Uniform
{
    std::string name;
    int location;

    Uniform()
        : name("<unknown>")
        , location(-1)
    {
    }

    Uniform(const std::string& n, int l)
        : name(n)
        , location(l)
    {
    }

    operator bool() const
    {
        return IsValid();
    }

    bool
    IsValid() const
    {
        return location >= 0;
    }
};


struct Shader
{
    Shader(std::string_view vertex_source, std::string_view fragment_source)
        : shader_program(glCreateProgram())
    {
        const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        UploadShaderSource(vertex_shader, vertex_source);
        glCompileShader(vertex_shader);
        const auto vertex_ok = CheckShaderCompilationError("vertex", vertex_shader);

        const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        UploadShaderSource(fragment_shader, fragment_source);
        glCompileShader(fragment_shader);
        const auto fragment_ok = CheckShaderCompilationError("fragment", fragment_shader);

        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        const auto link_ok = CheckShaderLinkError(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        glUseProgram(0);

        if(vertex_ok && fragment_ok && link_ok)
        {
            // nop
        }
        else
        {
            Delete();
        }
    }

    void
    Use() const
    {
        glUseProgram(shader_program);
    }

    void
    Delete()
    {
        glUseProgram(0);
        glDeleteProgram(shader_program);
        shader_program = 0;
    }

    Uniform
    GetUniform(const std::string& name) const
    {
        const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str())};
        if(uni.IsValid() == false)
        {
            SDL_Log("Uniform %s not found", name.c_str());
        }
        return uni;
    }

    // shader neeeds to be bound
    void
    SetFloat(const Uniform& uniform, float value) const
    {
        if(uniform.IsValid() == false) { return; }
        glUniform1f(uniform.location, value);
    }

    unsigned int shader_program;
};


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
    auto shader = Shader{VERTEX_GLSL, FRAGMENT_GLSL};
    auto uni = shader.GetUniform("dog");

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

    // defined in glsl
    const auto vertex_position_location = 0;

    const auto vbo = CreateBuffer();
    const auto vao = CreateVertexArray();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_position_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // class: use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = CreateBuffer();
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

        shader.Use();
        shader.SetFloat(uni, 2.0f);
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

    shader.Delete();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

