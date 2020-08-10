#include "glad/glad.h"
#include "SDL.h"



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

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // world.Render(viewport_handler.GetFullViewport(), camera);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

