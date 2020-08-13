#include "glad/glad.h"
#include "SDL.h"

#include <string_view>


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}

const char*
OpenglErrorToString(GLenum error_code)
{
    switch(error_code)
    {
    case GL_INVALID_ENUM: return "INVALID_ENUM"; break;
    case GL_INVALID_VALUE: return "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: return "INVALID_OPERATION"; break;
#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW: return "STACK_OVERFLOW"; break;
#endif
#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW"; break;
#endif
    case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "INVALID_FRAMEBUFFER_OPERATION";
        break;
    default: return "UNKNOWN"; break;
    }
}

namespace
{
    const char*
    SourceToString(GLenum source)
    {
        switch(source)
        {
        case GL_DEBUG_SOURCE_API_ARB: return "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            return "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            return "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: return "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB: return "Application"; break;
        case GL_DEBUG_SOURCE_OTHER_ARB: return "Other"; break;
        default: return "Unknown";
        }
    }

    const char*
    TypeToString(GLenum type)
    {
        switch(type)
        {
        case GL_DEBUG_TYPE_ERROR_ARB: return "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            return "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            return "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB: return "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB: return "Performance"; break;
        case GL_DEBUG_TYPE_OTHER_ARB: return "Other"; break;
        default: return "Unknown";
        }
    }

    const char*
    SeverityToString(GLenum severity)
    {
        switch(severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_ARB: return "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB: return "low"; break;
        default: return "unknown";
        }
    }

}  // namespace

    void APIENTRY
    OnOpenglError
    (
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei /*length*/,
            const GLchar* message,
            const GLvoid* /*userParam*/
    )
    {
        // ignore non-significant error/warning codes
        if(type == GL_DEBUG_TYPE_OTHER_ARB)
        {
            return;
        }

        // only display the first 10
        static int ErrorCount = 0;
        if(ErrorCount > 10)
        {
            return;
        }
        ++ErrorCount;

        SDL_Log("---------------");
        SDL_Log("Debug message (%d): %s", id, message);
        SDL_Log
        (
            "Source %s type: %s Severity: %s",
            SourceToString(source),
            TypeToString(type),
            SeverityToString(severity)
        );
        // ASSERT(false);
    }



void
SetupOpenglDebug()
{
    const bool has_debug = GLAD_GL_ARB_debug_output == 1;
    if(has_debug)
    {
        SDL_Log("Enabling OpenGL debug output");
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(OnOpenglError, nullptr);
        glDebugMessageControlARB(
                GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                nullptr,
                GL_TRUE);
    }
}


int
main(int, char**)
{
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
        "TRED",
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

    auto update_viewport = [&]()
    {
        glViewport(0, 0, width, height);
    };

    update_viewport();

    constexpr float vertices[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
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

    const auto vbo = generate_buffer();
    const auto vao = generate_vertex_array();
    glBindVertexArray(vao);

    // defined in glsl
    const auto vertex_position_location = 0;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_position_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    constexpr std::string_view vertex_shader_source =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    constexpr std::string_view fragment_shader_source =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";
    
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
    upload_shader_source(vertex_shader, vertex_shader_source);
    glCompileShader(vertex_shader);
    check_shader_compilation_error("vertex", vertex_shader);

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    upload_shader_source(fragment_shader, fragment_shader_source);
    glCompileShader(fragment_shader);
    check_shader_compilation_error("fragment", fragment_shader);

    const auto shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    check_shader_link_error(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(shader_program);

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
                    // todo
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

        // world.Render(viewport_handler.GetFullViewport(), camera);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);

    glUseProgram(0);
    glDeleteProgram(shader_program);

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

