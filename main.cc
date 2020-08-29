// standard headers
#include <string>
#include <string_view>
#include <cassert>
#include <vector>
#include <numeric>
#include <functional>

// dependency headers
#include "glad/glad.h"
#include "SDL.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "debug_opengl.h"

// resource headers
#include "vertex.glsl.h"
#include "fragment.glsl.h"
#include "container.jpg.h"
#include "awesomeface.png.h"

///////////////////////////////////////////////////////////////////////////////
// common "header"


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}


std::size_t
Cint_to_sizet(int i)
{
    return static_cast<std::size_t>(i);
}

int
Cunsigned_int_to_int(unsigned int ui)
{
    return static_cast<int>(ui);
}

///////////////////////////////////////////////////////////////////////////////
// mesh header


struct Vertex
{
    glm::vec3 position;
    glm::vec2 texture;
    glm::vec4 color;

    Vertex
    (
        const glm::vec3& p,
        const glm::vec2& t,
        const glm::vec4& c = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
    )
        : position(p)
        , texture(t)
        , color(c)
    {
    }
};


using Triangle = glm::ivec3;


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> triangles; // %3 == 0

    Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& tris)
        : vertices(verts)
        , triangles(tris)
    {
        assert(triangles.size() % 3 == 0);
    }
};


///////////////////////////////////////////////////////////////////////////////
// vertex layout

enum class VertexType
{
    Position3, Color4, Texture2
};


struct VertexElement
{
    VertexType type;
    std::string name;

    VertexElement(VertexType t, const std::string& n)
        : type(t)
        , name(n)
    {
    }
};


using VertexLayout = std::vector<VertexElement>;


///////////////////////////////////////////////////////////////////////////////
// rendering functions

unsigned int CreateTexture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}


enum class TextureEdge
{
    Clamp, Repeat
};


enum class TextureRenderStyle
{
    Pixel, Smooth
};


enum class Transperency
{
    Include, Exclude
};


unsigned int
LoadImage
(
    const unsigned char* image_source,
    int size,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    const auto texture = CreateTexture();
    glBindTexture(GL_TEXTURE_2D, texture);

    const auto wrap = texture_edge == TextureEdge::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    const auto render_pixels = texture_render_style == TextureRenderStyle::Pixel;

    const auto min_filter = render_pixels ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    const auto mag_filter = render_pixels ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    int width = 0;
    int height = 0;
    int junk_channels;
    stbi_set_flip_vertically_on_load(true);
    const auto include_transperency = transperency == Transperency::Include;
    auto* pixel_data = stbi_load_from_memory
    (
        image_source, size,
        &width, &height,
        &junk_channels, include_transperency ? 4 : 3
    );

    if(pixel_data == nullptr)
    {
        SDL_Log("ERROR: Failed to load image.");
    }
    else
    {
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            include_transperency ? GL_RGBA : GL_RGB,
            width, height,
            0,
            include_transperency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
            pixel_data
        );
        if(render_pixels == false)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(pixel_data);
    }

    return texture;
}


unsigned int
LoadImageEmbeded
(
    const unsigned int* source, unsigned int size,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    return LoadImage
    (
        reinterpret_cast<const unsigned char*>(source),
        Cunsigned_int_to_int(size),
        texture_edge,
        texture_render_style,
        transperency
    );
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


unsigned int
CreateBuffer()
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    return buffer;
};


unsigned int
CreateVertexArray()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    return vao;
};


struct Uniform
{
    std::string name;
    int location;
    int texture = -1; // >=0 if this is uniform maps to a texture

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


int
ShaderAttributeSize(const VertexElement&)
{
    return 1;
}


void
BindShaderAttributeLocation(unsigned int shader_program, const VertexLayout& layout)
{
    int requested_index = 0;
    for(const auto& b: layout)
    {
        glBindAttribLocation(shader_program, requested_index, b.name.c_str());
        requested_index += ShaderAttributeSize(b);
    }
}


void
VerifyShaderAttributeLocation(unsigned int shader_program, const VertexLayout& layout)
{
    int requested_index = 0;
    for(const auto& b: layout)
    {
        const auto actual_index = glGetAttribLocation
        (
            shader_program,
            b.name.c_str()
        );

        if(actual_index != requested_index)
        {
            SDL_Log
            (
                "ERROR: %s was bound to %d but requested at %d",
                b.name.c_str(),
                actual_index,
                requested_index
            );
        }

        requested_index += ShaderAttributeSize(b);
    }
}


unsigned int debug_current_shader_program;
void
SetShaderProgram(unsigned int new_program)
{
    debug_current_shader_program = new_program;
    glUseProgram(new_program);
}


struct Shader
{
    Shader
    (
        std::string_view vertex_source,
        std::string_view fragment_source,
        const VertexLayout& layout
    )
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
        BindShaderAttributeLocation(shader_program, layout);
        glLinkProgram(shader_program);
        const auto link_ok = CheckShaderLinkError(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        SetShaderProgram(0);

        if(vertex_ok && fragment_ok && link_ok)
        {
            // nop
            VerifyShaderAttributeLocation(shader_program, layout);
        }
        else
        {
            Delete();
        }
    }

    void
    Use() const
    {
        SetShaderProgram(shader_program);
    }

    void
    Delete()
    {
        SetShaderProgram(0);
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
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.texture == -1 && "uniform is a texture not a float");
        glUniform1f(uniform.location, value);
    }

    void
    SetVec4(const Uniform& uniform, float x, float y, float z, float w)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.texture == -1 && "uniform is a texture not a vec4");
        glUniform4f(uniform.location, x, y, z, w);
    }

    void
    SetTexture(const Uniform& uniform)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.texture >= 0 && "uniform needs to be a texture");
        glUniform1i(uniform.location, uniform.texture);
    }

    void
    SetMat(const Uniform& uniform, const glm::mat4& mat)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.texture == -1 && "uniform is a texture not a matrix");
        glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    unsigned int shader_program;
};


void
SetupTextures(Shader* shader, std::vector<Uniform*> uniform_list)
{
    // OpenGL should support atleast 16 textures
    assert(uniform_list.size() <= 16);

    shader->Use();

    int index = 0;
    for(const auto& uniform: uniform_list)
    {
        uniform->texture = index;
        index += 1;
        shader->SetTexture(*uniform);
    }
}


void
BindTexture(const Uniform& uniform, unsigned int texture)
{
    if(uniform.IsValid() == false) { return; }
    assert(uniform.texture >= 0);

    glActiveTexture(GL_TEXTURE0 + uniform.texture);
    glBindTexture(GL_TEXTURE_2D, texture);
}



struct CompiledMesh
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int number_of_indices;

    unsigned int debug_shader_program;

    CompiledMesh(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, unsigned int sp)
         : vbo(a_vbo)
         , vao(a_vao)
         , ebo(a_ebo)
         , number_of_indices(count)
         , debug_shader_program(sp)
    {
        assert(sp != 0);
    }

    void
    Draw() const
    {
        assert(debug_shader_program == debug_current_shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void
    Delete() const
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ebo);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vbo);
    }
};


struct BufferData
{
    using PerVertex = std::function<void (std::vector<float>*, const Vertex&)>;

    int count;
    PerVertex per_vertex;
    int start = 0;

    BufferData(int c, PerVertex pv)
        : count(c)
        , per_vertex(pv)
    {
    }
};


CompiledMesh
Compile(const Mesh& mesh, const Shader& shader, const VertexLayout& layout)
{
    using VertexVector = std::vector<float>;

    auto data = std::vector<BufferData>{};
    data.reserve(layout.size());

    for(const auto& element: layout)
    {
        switch(element.type)
        {
        case VertexType::Position3:
            data.emplace_back(3, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.position.x);
                vertices->push_back(vertex.position.y);
                vertices->push_back(vertex.position.z);
            });
            break;
        case VertexType::Color4:
            data.emplace_back(4, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.color.x);
                vertices->push_back(vertex.color.y);
                vertices->push_back(vertex.color.z);
                vertices->push_back(vertex.color.w);
            });
            break;
        case VertexType::Texture2:
            data.emplace_back(2, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.texture.x);
                vertices->push_back(vertex.texture.y);
            });
            break;
        default:
            assert(false && "unhandled buffer type");
            break;
        }
    }

    const auto floats_per_vertex = Cint_to_sizet
    (
        std::accumulate
        (
            data.begin(), data.end(),
            0, [](auto s, const auto& d)
            {
                return s + d.count;
            }
        )
    );
    auto vertices = VertexVector{};
    vertices.reserve(mesh.vertices.size() * floats_per_vertex);
    for(const auto& vertex: mesh.vertices)
    for(const auto& d: data)
    {
        d.per_vertex(&vertices, vertex);
    }

    const auto vbo = CreateBuffer();
    const auto vao = CreateVertexArray();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        &vertices[0],
        GL_STATIC_DRAW
    );

    const auto stride = floats_per_vertex * sizeof(float);
    int location = 0;
    std::size_t offset = 0;
    for(const auto& d: data)
    {
        const auto normalize = false;
        glVertexAttribPointer
        (
            location,
            d.count,
            GL_FLOAT,
            normalize ? GL_TRUE : GL_FALSE,
            stride,
            reinterpret_cast<void*>(offset)
        );
        glEnableVertexAttribArray(location);

        location += 1;
        offset += Cint_to_sizet(d.count) * sizeof(float);
    }

    // class: use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = CreateBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.triangles.size() * sizeof(unsigned int),
        &mesh.triangles[0],
        GL_STATIC_DRAW
    );

    return CompiledMesh{vbo, vao, ebo, Csizet_to_int(mesh.triangles.size()), shader.shader_program};
}


Mesh
CreatePlaneMesh()
{
    return
    {
        {
            {{ 0.5f, -0.5f, 0.0f}, {1, 0}},
            {{ 0.5f,  0.5f, 0.0f}, {1, 1}, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0, 0}},
            {{-0.5f,  0.5f, 0.0f}, {0, 1}}
        },
        {
            0, 1, 3,
            1, 2, 3
        }
    };
}


Mesh
CreateBoxMesh()
{
    return
    {
        {
            {{ 0.5f, -0.5f, -0.5f}, { 1.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 0.0f}},

            {{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, { 0.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}},

            {{-0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},

            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},

            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{ 0.5f, -0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}},

            {{-0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}}
        },
        {
            0,  1,  2,  3,  4,  5 ,
            6,  7,  8,  9,  10, 11,
            12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23,
            24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35
        }
    };
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

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        SDL_Log("Failed to initialize OpenGL context");
        return -1;
    }

    // setup OpenGL
    SetupOpenglDebug();
    glEnable(GL_DEPTH_TEST);

    const auto* renderer = glGetString(GL_RENDERER); // get renderer string
    const auto* version = glGetString(GL_VERSION); // version as a string
    SDL_Log("Renderer: %s\n", renderer);
    SDL_Log("Version: %s\n", version);

    // imgui setup
    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsLight();
    
    ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
    ImGui_ImplOpenGL3_Init(glsl_version);

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

    ///////////////////////////////////////////////////////////////////////////
    // shader layout
    const auto layout = VertexLayout
    {
        {VertexType::Position3, "aPos"},
        {VertexType::Color4, "aColor"},
        {VertexType::Texture2, "aTexCoord"}
    };

    ///////////////////////////////////////////////////////////////////////////
    // shaders
    auto shader = Shader{VERTEX_GLSL, FRAGMENT_GLSL, layout};
    const auto uni_color = shader.GetUniform("uColor");
    auto uni_texture = shader.GetUniform("uTexture");
    auto uni_decal = shader.GetUniform("uDecal");
    const auto uni_transform = shader.GetUniform("uTransform");
    SetupTextures(&shader, {&uni_texture, &uni_decal});

    ///////////////////////////////////////////////////////////////////////////
    // model
    const auto mesh = Compile(CreateBoxMesh(), shader, layout);

    const auto texture = LoadImageEmbeded
    (
        CONTAINER_JPG_data, CONTAINER_JPG_size,
        TextureEdge::Clamp,
        TextureRenderStyle::Smooth,
        Transperency::Exclude
    );

    const auto awesome = LoadImageEmbeded
    (
        AWESOMEFACE_PNG_data, AWESOMEFACE_PNG_size,
        TextureEdge::Clamp,
        TextureRenderStyle::Smooth,
        Transperency::Include
    );

    ///////////////////////////////////////////////////////////////////////////
    // view
    // const auto projection_ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glm::mat4 projection;
    auto fov = 45.0f;

    const auto update_viewport = [&]()
    {
        glViewport(0, 0, width, height);
        const auto aspect_ratio = static_cast<float>(width)/static_cast<float>(height);
        projection = glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
    };

    update_viewport();

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

    ///////////////////////////////////////////////////////////////////////////
    // main

    bool running = true;
    auto last = SDL_GetPerformanceCounter();
    auto time = 0.0f;

    auto capture_input = false;
    const auto set_capture_input = [&capture_input](bool value)
    {
        capture_input = value;
        SDL_SetRelativeMouseMode(capture_input ? SDL_TRUE : SDL_FALSE);
    };
    auto input_w = false;
    auto input_a = false;
    auto input_s = false;
    auto input_d = false;
    auto input_shift = false;

    auto mouse_left = false;
    auto mouse_middle = false;
    auto mouse_right = false;

    auto camera_position = glm::vec3{0.0f, 0.0f,  3.0f};
    auto camera_pitch = 0.0f;
    auto camera_yaw = -90.0f;

    while(running)
    {
        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        auto input_mouse = glm::vec2{0, 0};

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(!capture_input)
            {
                ImGui_ImplSDL2_ProcessEvent(&e);
            }
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
                input_mouse.x += static_cast<float>(e.motion.xrel);
                input_mouse.y += static_cast<float>(e.motion.yrel);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                const bool down = e.type == SDL_MOUSEBUTTONDOWN;

                switch(e.button.button)
                {
                    case SDL_BUTTON_LEFT: mouse_left = down; break;
                    case SDL_BUTTON_MIDDLE: mouse_middle = down; break;
                    case SDL_BUTTON_RIGHT: mouse_right = down; break;
                    default:
                        // ignore other mouse buttons
                        break;
                }
            }
            break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                const bool down = e.type == SDL_KEYDOWN;

                switch(e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    if(!down)
                    {
                        if(capture_input)
                        {
                            set_capture_input(false);
                        }
                        else
                        {
                            running = false;
                        }
                    }
                    break;
                case SDLK_w: input_w = down; break;
                case SDLK_a: input_a = down; break;
                case SDLK_s: input_s = down; break;
                case SDLK_d: input_d = down; break;
                case SDLK_LSHIFT: case SDLK_RSHIFT: input_shift = down; break;
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

        constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);
        const auto direction = glm::vec3
        {
            cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch)),
            sin(glm::radians(camera_pitch)),
            sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch))
        };
        const auto camera_front = glm::normalize(direction);
        const auto camera_right = glm::normalize(glm::cross(camera_front, UP));
        const auto camera_up = glm::normalize(glm::cross(camera_right, camera_front));

        // handle mouse input
        if(capture_input)
        {
            const float sensitivity = 0.1f;
            camera_yaw   += input_mouse.x * sensitivity;
            camera_pitch -= input_mouse.y * sensitivity;
        }
        else if(io.WantCaptureMouse == false)
        {
            const float move_sensitivity = 0.01f;
            if(mouse_left)
            {
                // todo(Gustav): make actually roatate around a center point instead
                const float sensitivity = 0.1f;
                camera_yaw   += input_mouse.x * sensitivity;
                camera_pitch -= input_mouse.y * sensitivity;
            }
            else if(mouse_middle)
            {
                camera_position +=
                      camera_right * input_mouse.x * move_sensitivity
                    - camera_up * input_mouse.y * move_sensitivity;
            }
            else if(mouse_right)
            {
                camera_position += camera_front * input_mouse.y * move_sensitivity;
            }
        }

        if(camera_pitch >  89.0f) camera_pitch =  89.0f;
        if(camera_pitch < -89.0f) camera_pitch = -89.0f;

        // handle keyboard input
        if(capture_input)
        {
            const auto camera_speed = 3 * dt * (input_shift ? 2.0f : 1.0f);
            if (input_w) camera_position += camera_speed * camera_front;
            if (input_s) camera_position -= camera_speed * camera_front;
            if (input_a) camera_position -= camera_speed * camera_right;
            if (input_d) camera_position += camera_speed * camera_right;
        }

        time += dt;
        const auto pi = 3.14f;
        if(time > 2*pi) { time -= 2* pi; }

        const auto view = glm::lookAt(camera_position, camera_position + camera_front, UP);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        shader.Use();
        BindTexture(uni_texture, texture);
        BindTexture(uni_decal, awesome);
        shader.SetVec4(uni_color, 1.0f, 1.0f, 1.0f, 1.0f);

        const auto pv = projection * view;
        for(unsigned int i=0; i<cube_positions.size(); i+=1)
        {
            const auto angle = 20.0f * static_cast<float>(i);
            const auto model = glm::rotate
            (
                glm::translate(glm::mat4(1.0f), cube_positions[i]),
                time + glm::radians(angle),
                i%2 == 0
                ? glm::vec3{1.0f, 0.3f, 0.5f}
                : glm::vec3{0.5f, 1.0f, 0.0f}
            );

            shader.SetMat(uni_transform, pv * model);
            mesh.Draw();
        }

        if(capture_input == false)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // ImGui::ShowDemoWindow();

            if(ImGui::Begin("Debug"))
            {
                if(ImGui::Button("Quit")) { running = false; }
                ImGui::SameLine();
                if(ImGui::Button("FPS controller"))
                {
                    set_capture_input(true);
                }
                if(ImGui::Combo("Rendering mode", &rendering_mode, "Fill\0Line\0Point\0"))
                {
                    set_rendering_mode();
                }
                if(ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 145.0f))
                {
                    update_viewport();
                }

                if (ImGui::CollapsingHeader("Cube positions"))
                {
                    for(auto& cube: cube_positions)
                    {
                        ImGui::PushID(&cube);
                        ImGui::DragFloat3("", glm::value_ptr(cube), 0.01f);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(window);
    }

    mesh.Delete();
    shader.Delete();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

