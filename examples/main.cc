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

#include <fmt/core.h>

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
#include "tred/debug_opengl.h"
#include "tred/profiler.h"
#include "tred/cint.h"

// resource headers
#include "shader_vertex.glsl.h"
#include "shader_fragment.glsl.h"
#include "light_vertex.glsl.h"
#include "light_fragment.glsl.h"
#include "container_diffuse.png.h"
#include "container_specular.png.h"


///////////////////////////////////////////////////////////////////////////////
// mesh header


/** A unique combination of position/normal/texturecoord/etc. in a Mesh.
 */
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;
    glm::vec4 color;

    Vertex
    (
        const glm::vec3& p,
        const glm::vec3& n,
        const glm::vec2& t,
        const glm::vec4& c = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
    )
        : position(p)
        , normal(n)
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

/** Vertex source type, position, normal etc. */
enum class VertexType
{
    Position3, Normal3, Color4, Texture2

    // change to include other textcoords and custom types that are created from scripts
};


/** A not-yet-realised binding to a shader variable like 'vec3 position' */
struct VertexElementDescription
{
    VertexType type;
    std::string name;

    VertexElementDescription(VertexType t, const std::string& n)
        : type(t)
        , name(n)
    {
    }
};

/** A realized VertexElementDescription */
struct CompiledVertexElement
{
    VertexType type;
    std::string name;
    int index;

    CompiledVertexElement(const VertexElementDescription& d, int i)
        : type(d.type)
        , name(d.name)
        , index(i)
    {
    }
};


using VertexLayoutDescription = std::vector<VertexElementDescription>;
using VertexTypes = std::vector<VertexType>;

/** A list of CompiledVertexElement */
struct CompiledVertexLayout
{
    using CompiledVertexLayoutList = std::vector<CompiledVertexElement>;

    CompiledVertexLayout(const CompiledVertexLayoutList& e, const VertexTypes& t)
        : elements(e)
        , types(t)
    {
    }

    CompiledVertexLayoutList elements;
    VertexTypes types;
    
    // todo(Gustav): isn't the type variable a copy of elements.type?
};

/** A list of things we need to extract from the Mesh when compiling */
struct VertexTypeList
{
    void
    Add(const VertexLayoutDescription& elements)
    {
        for(const auto& e: elements)
        {
            indices.insert(e.type);
        }
    }

    std::set<VertexType> indices;
};

/** A mapping of the vertex type (position...) to the actual shader id */
struct CompiledVertexTypeList
{
    CompiledVertexTypeList(const std::map<VertexType, int>& i, const VertexTypes& v)
        : indices(i)
        , vertex_types(v)
    {
    }

    CompiledVertexLayout
    Compile(const VertexLayoutDescription& elements) const
    {
        CompiledVertexLayout::CompiledVertexLayoutList list;

        for(const auto& e: elements)
        {
            const auto found = indices.find(e.type);
            assert(found != indices.end() && "layout wasn't added to the compilation list");

            list.push_back({e, found->second});
        }

        return {list, vertex_types};
    }

    std::map<VertexType, int> indices;
    VertexTypes vertex_types;
};


int
ShaderAttributeSize(const VertexType&)
{
    return 1;
}


CompiledVertexTypeList
Compile(const VertexTypeList& list)
{
    std::map<VertexType, int> indices;

    int next_index = 0;
    for(const auto type: list.indices)
    {
        indices[type] = next_index;
        next_index += ShaderAttributeSize(type);
    }

    return {indices, {list.indices.begin(), list.indices.end()} };
}


CompiledVertexTypeList
Compile(const std::vector<VertexLayoutDescription>& descriptions)
{
    VertexTypeList list;

    for(const auto& d: descriptions)
    {
        list.Add(d);
    }

    return Compile(list);
}


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
        const auto msg = fmt::format("ERROR: {} shader compilation failed\n{}}\n", name, log);
        SDL_Log("%s", msg.c_str());
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


/** Represents a found shader uniform and created via Shader::GetUniform()
 */
struct Uniform
{
    std::string name;
    int location;
    unsigned int debug_shader_program;
    int texture = -1; // >=0 if this is uniform maps to a texture

    Uniform()
        : name("<unknown>")
        , location(-1)
        , debug_shader_program(0)
    {
    }

    Uniform(const std::string& n, int l, unsigned int sp)
        : name(n)
        , location(l)
        , debug_shader_program(sp)
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


void
BindShaderAttributeLocation(unsigned int shader_program, const CompiledVertexLayout& layout)
{
    for(const auto& b: layout.elements)
    {
        glBindAttribLocation(shader_program, Cint_to_gluint(b.index), b.name.c_str());
    }
}


void
VerifyShaderAttributeLocation(unsigned int shader_program, const CompiledVertexLayout& layout)
{
    for(const auto& b: layout.elements)
    {
        const auto actual_index = glGetAttribLocation
        (
            shader_program,
            b.name.c_str()
        );

        if(actual_index != b.index)
        {
            SDL_Log
            (
                "ERROR: %s was bound to %d but requested at %d",
                b.name.c_str(),
                actual_index,
                b.index
            );
        }
    }
}


VertexTypes debug_current_shader_types;
unsigned int debug_current_shader_program;
void
SetShaderProgram(unsigned int new_program, const VertexTypes& types)
{
    debug_current_shader_program = new_program;
    debug_current_shader_types = types;
    glUseProgram(new_program);
}

void
ClearShaderProgram()
{
    SetShaderProgram(0, {});
}


struct Shader
{
    Shader
    (
        std::string_view vertex_source,
        std::string_view fragment_source,
        const CompiledVertexLayout& layout
    )
        : shader_program(glCreateProgram())
        , vertex_types(layout.types)
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

        ClearShaderProgram();

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
        SetShaderProgram(shader_program, vertex_types);
    }

    void
    Delete()
    {
        ClearShaderProgram();
        glDeleteProgram(shader_program);
        shader_program = 0;
    }

    Uniform
    GetUniform(const std::string& name) const
    {
        const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str()), shader_program};
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
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture == -1 && "uniform is a texture not a float");
        glUniform1f(uniform.location, value);
    }

    void
    SetVec3(const Uniform& uniform, float x, float y, float z)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture == -1 && "uniform is a texture not a vec3");
        glUniform3f(uniform.location, x, y, z);
    }

    void
    SetVec3(const Uniform& uniform, const glm::vec3& v)
    {
        SetVec3(uniform, v.x, v.y, v.z);
    }

    void
    SetVec4(const Uniform& uniform, float x, float y, float z, float w)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture == -1 && "uniform is a texture not a vec4");
        glUniform4f(uniform.location, x, y, z, w);
    }

    void
    SetVec4(const Uniform& uniform, const glm::vec4& v)
    {
        SetVec4(uniform, v.x, v.y, v.z, v.w);
    }

    void
    SetTexture(const Uniform& uniform)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture >= 0 && "uniform needs to be a texture");
        glUniform1i(uniform.location, uniform.texture);
    }

    void
    SetMat(const Uniform& uniform, const glm::mat4& mat)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture == -1 && "uniform is a texture not a matrix4");
        glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void
    SetMat(const Uniform& uniform, const glm::mat3& mat)
    {
        assert(debug_current_shader_program == shader_program);
        if(uniform.IsValid() == false) { return; }
        assert(uniform.debug_shader_program == shader_program);

        assert(uniform.texture == -1 && "uniform is a texture not a matrix3");
        glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    unsigned int shader_program;
    VertexTypes vertex_types;
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


struct Texture
{
    unsigned int id;
    explicit Texture(unsigned int i) : id(i) {}

    void
    Delete()
    {
        // todo(Gustav): implement this
    }
};


void
BindTexture(const Uniform& uniform, const Texture& texture)
{
    if(uniform.IsValid() == false) { return; }
    assert(uniform.texture >= 0);

    glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
    glBindTexture(GL_TEXTURE_2D, texture.id);
}



/** A compiled Mesh*/
struct CompiledMesh
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int number_of_indices;

    VertexTypes debug_shader_types;

    CompiledMesh(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const VertexTypes& st)
         : vbo(a_vbo)
         , vao(a_vao)
         , ebo(a_ebo)
         , number_of_indices(count)
         , debug_shader_types(st)
    {
    }

    void
    Draw() const
    {
        assert(debug_shader_types == debug_current_shader_types);
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


/** Internal class used when compiling Mesh.
 */
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
Compile(const Mesh& mesh, const CompiledVertexLayout& layout)
{
    using VertexVector = std::vector<float>;

    auto data = std::vector<BufferData>{};
    data.reserve(layout.elements.size());

    for(const auto& element: layout.elements)
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
        case VertexType::Normal3:
            data.emplace_back(3, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.normal.x);
                vertices->push_back(vertex.normal.y);
                vertices->push_back(vertex.normal.z);
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
        Csizet_to_glsizeiptr(vertices.size() * sizeof(float)),
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
            Cint_to_gluint(location),
            d.count,
            GL_FLOAT,
            normalize ? GL_TRUE : GL_FALSE,
            Csizet_to_glsizei(stride),
            reinterpret_cast<void*>(offset)
        );
        glEnableVertexAttribArray(Cint_to_gluint(location));

        location += 1;
        offset += Cint_to_sizet(d.count) * sizeof(float);
    }

    // class: use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = CreateBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        Csizet_to_glsizeiptr(mesh.triangles.size() * sizeof(unsigned int)),
        &mesh.triangles[0],
        GL_STATIC_DRAW
    );

    return CompiledMesh{vbo, vao, ebo, Csizet_to_int(mesh.triangles.size()), layout.types};
}


Mesh
CreatePlaneMesh()
{
    return
    {
        {
            {{ 0.5f, -0.5f, 0.0f}, {0, 1, 0}, {1, 0}},
            {{ 0.5f,  0.5f, 0.0f}, {0, 1, 0}, {1, 1}, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0, 1, 0}, {0, 0}},
            {{-0.5f,  0.5f, 0.0f}, {0, 1, 0}, {0, 1}}
        },
        {
            0, 1, 3,
            1, 2, 3
        }
    };
}


Mesh
CreatePlaneMesh(float size, float uv)
{
    const auto h = size / 2.0f;
    return
    {
        {
            {{-h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f,   uv}},
            {{ h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, {   uv,   uv}},
            {{ h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, {   uv, 0.0f}},
            {{ h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, {   uv, 0.0f}},
            {{-h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f}},
            {{-h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f,   uv}}
        },
        {
             0,  1,  2,    3,  4,  5
        }
    };
}


Mesh
CreateBoxMesh(float size)
{
    const auto h = size / 2.0f;
    return
    {
        {
            {{ h, -h, -h}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 0.0f}},
            {{-h, -h, -h}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}},
            {{ h,  h, -h}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f}},
            {{ h,  h, -h}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f}},
            {{-h,  h, -h}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 1.0f}},
            {{-h, -h, -h}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}},

            {{-h, -h,  h}, { 0.0f,  0.0f, 1.0f}, { 0.0f, 0.0f}},
            {{ h, -h,  h}, { 0.0f,  0.0f, 1.0f}, { 1.0f, 0.0f}},
            {{ h,  h,  h}, { 0.0f,  0.0f, 1.0f}, { 1.0f, 1.0f}},
            {{ h,  h,  h}, { 0.0f,  0.0f, 1.0f}, { 1.0f, 1.0f}},
            {{-h,  h,  h}, { 0.0f,  0.0f, 1.0f}, { 0.0f, 1.0f}},
            {{-h, -h,  h}, { 0.0f,  0.0f, 1.0f}, { 0.0f, 0.0f}},

            {{-h,  h,  h}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},
            {{-h,  h, -h}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}},
            {{-h, -h, -h}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{-h, -h, -h}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{-h, -h,  h}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}},
            {{-h,  h,  h}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},

            {{ h,  h,  h}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},
            {{ h,  h, -h}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}},
            {{ h, -h, -h}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ h, -h, -h}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ h, -h,  h}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}},
            {{ h,  h,  h}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},

            {{-h, -h, -h}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ h, -h, -h}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 1.0f}},
            {{ h, -h,  h}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{ h, -h,  h}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{-h, -h,  h}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 0.0f}},
            {{-h, -h, -h}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f}},

            {{-h,  h, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ h,  h, -h}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 1.0f}},
            {{ h,  h,  h}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{ h,  h,  h}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{-h,  h,  h}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f}},
            {{-h,  h, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f}}
        },
        {
             0,  1,  2,    3,  4,  5,
             6,  7,  8,    9, 10, 11,
            12, 13, 14,   15, 16, 17,
            18, 19, 20,   21, 22, 23,
            24, 25, 26,   27, 28, 29,
            30, 31, 32,   33, 34, 35
        }
    };
}


struct Material
{
    Texture diffuse;
    Texture specular;

    glm::vec3 tint = glm::vec3{1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;

    float specular_strength = 1.0f;

    explicit Material(const Texture& d, const Texture& s)
        : diffuse(d)
        , specular(s)
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
        Shader* shader,
        const std::string& base_name
    )
        : diffuse(shader->GetUniform(base_name + ".diffuse"))
        , specular(shader->GetUniform(base_name + ".specular"))
        , tint(shader->GetUniform(base_name + ".tint"))
        , shininess(shader->GetUniform(base_name + ".shininess"))
        , specular_strength(shader->GetUniform(base_name + ".specular_strength" ))
    {
        SetupTextures(shader, {&diffuse, &specular});
    }

    void
    SetShader(Shader* shader, const Material& material) const
    {
        BindTexture(diffuse, material.diffuse);
        BindTexture(specular, material.specular);

        shader->SetVec3(tint, material.tint);
        shader->SetFloat(specular_strength, material.specular_strength);
        shader->SetFloat(shininess, material.shininess);
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
        const Shader& shader,
        const std::string& base_name
    )
    :
        direction(shader.GetUniform(base_name + ".normalized_direction")),
        ambient(shader.GetUniform(base_name + ".ambient")),
        diffuse(shader.GetUniform(base_name + ".diffuse")),
        specular(shader.GetUniform(base_name + ".specular"))
    {
    }

    void
    SetShader(Shader* shader, const DirectionalLight& light) const
    {
        shader->SetVec3(direction, light.GetDirection());
        shader->SetVec3(ambient, light.ambient * light.ambient_strength);
        shader->SetVec3(diffuse, light.diffuse);
        shader->SetVec3(specular, light.specular);
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
        const Shader& shader,
        const std::string& base_name
    )
    :
        constant(shader.GetUniform(base_name + ".constant")),
        linear(shader.GetUniform(base_name + ".linear")),
        quadratic(shader.GetUniform(base_name + ".quadratic"))
    {
    }

    void
    SetShader(Shader* shader, const Attenuation& att) const
    {
        shader->SetFloat(constant, att.constant);
        shader->SetFloat(linear, att.linear);
        shader->SetFloat(quadratic, att.quadratic);
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
        const Shader& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.GetUniform(base_name + ".position")),
        ambient(shader.GetUniform(base_name + ".ambient")),
        diffuse(shader.GetUniform(base_name + ".diffuse")),
        specular(shader.GetUniform(base_name + ".specular"))
    {
    }

    void
    SetShader(Shader* shader, const PointLight& light) const
    {
        attenuation.SetShader(shader, light.attenuation);
        shader->SetVec3(position, light.position);
        shader->SetVec3(ambient, light.ambient * light.ambient_strength);
        shader->SetVec3(diffuse, light.diffuse);
        shader->SetVec3(specular, light.specular);
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
        const Shader& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.GetUniform(base_name + ".position")),
        direction(shader.GetUniform(base_name + ".direction")),
        cos_cutoff(shader.GetUniform(base_name + ".cos_cutoff")),
        cos_outer_cutoff(shader.GetUniform(base_name + ".cos_outer_cutoff")),
        ambient(shader.GetUniform(base_name + ".ambient")),
        diffuse(shader.GetUniform(base_name + ".diffuse")),
        specular(shader.GetUniform(base_name + ".specular"))
    {
    }

    void
    SetShader(Shader* shader, const SpotLight& light) const
    {
        attenuation.SetShader(shader, light.attenuation);
        shader->SetVec3(position, light.position);
        shader->SetVec3(direction, light.direction);
        shader->SetFloat(cos_cutoff, cos(glm::radians(light.cutoff)));
        shader->SetFloat(cos_outer_cutoff, cos(glm::radians(light.outer_cutoff)));
        shader->SetVec3(ambient, light.ambient * light.ambient_strength);
        shader->SetVec3(diffuse, light.diffuse);
        shader->SetVec3(specular, light.specular);
    }
};


constexpr unsigned int NUMBER_OF_POINT_LIGHTS = 4;

int
main(int, char**)
{
    const auto pi = 3.14f;
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
    const auto layout = VertexLayoutDescription
    {
        {VertexType::Position3, "aPos"},
        {VertexType::Normal3, "aNormal"},
        {VertexType::Color4, "aColor"},
        {VertexType::Texture2, "aTexCoord"}
    };
    auto layout_compiler = Compile({layout});
    const auto compiled_layout = layout_compiler.Compile(layout);

    const auto light_layout = VertexLayoutDescription
    {
        {VertexType::Position3, "aPos"}
    };
    auto light_compiler = Compile({light_layout});
    const auto compiled_light_layout = light_compiler.Compile(light_layout);

    ///////////////////////////////////////////////////////////////////////////
    // shaders
    auto shader = Shader{SHADER_VERTEX_GLSL, SHADER_FRAGMENT_GLSL, compiled_layout};
    const auto uni_color = shader.GetUniform("uColor");
    const auto uni_transform = shader.GetUniform("uTransform");
    const auto uni_model_transform = shader.GetUniform("uModelTransform");
    const auto uni_normal_matrix = shader.GetUniform("uNormalMatrix");
    const auto uni_view_position = shader.GetUniform("uViewPosition");
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

    auto light_shader = Shader{LIGHT_VERTEX_GLSL, LIGHT_FRAGMENT_GLSL, compiled_light_layout};
    const auto uni_light_transform = light_shader.GetUniform("uTransform");
    const auto uni_light_color = light_shader.GetUniform("uColor");

    ///////////////////////////////////////////////////////////////////////////
    // model
    const auto mesh = Compile(CreateBoxMesh(1.0f), compiled_layout);
    const auto light_mesh = Compile(CreateBoxMesh(0.2f), compiled_light_layout);
    const auto plane_mesh = Compile(CreatePlaneMesh(20.0f, 20.0f), compiled_layout);

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
    auto cube_color = glm::vec4{1.0f};

    ///////////////////////////////////////////////////////////////////////////
    // main

    bool running = true;
    auto last = SDL_GetPerformanceCounter();
    auto time = 0.0f;
    bool animate = true;

    auto input_fps = false;
    const auto set_input_fps = [&input_fps](bool value)
    {
        input_fps = value;
        SDL_SetRelativeMouseMode(input_fps ? SDL_TRUE : SDL_FALSE);
    };
    auto input_w = false;
    auto input_a = false;
    auto input_s = false;
    auto input_d = false;
    auto input_space = false;
    auto input_ctrl = false;
    auto input_shift = false;

    auto mouse_left = false;
    auto mouse_middle = false;
    auto mouse_right = false;

    auto camera_position = glm::vec3{0.0f, 0.0f,  3.0f};
    auto camera_pitch = 0.0f;
    auto camera_yaw = -90.0f;

    auto material = Material
    {
        Texture
        {
            LoadImageEmbeded
            (
                CONTAINER_DIFFUSE_PNG_data, CONTAINER_DIFFUSE_PNG_size,
                TextureEdge::Repeat,
                TextureRenderStyle::Smooth,
                Transperency::Exclude
            )
        },
        Texture
        {
            LoadImageEmbeded
            (
                CONTAINER_SPECULAR_PNG_data, CONTAINER_SPECULAR_PNG_size,
                TextureEdge::Repeat,
                TextureRenderStyle::Smooth,
                Transperency::Exclude
            )
        }
    };
    auto directional_light = DirectionalLight{};
    auto point_lights = std::array<PointLight, NUMBER_OF_POINT_LIGHTS>
    {
        glm::vec3{ 0.7f,  0.2f,  2.0f},
        glm::vec3{ 2.3f, -3.3f, -4.0f},
        glm::vec3{-4.0f,  2.0f, -12.0f},
        glm::vec3{ 0.0f,  0.0f, -3.0f}
    };
    auto spot_light = SpotLight{};

    while(running)
    {
        PROFILE_FRAME();

        const auto now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = now;

        auto input_mouse = glm::vec2{0, 0};

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(!input_fps)
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
                        if(input_fps)
                        {
                            set_input_fps(false);
                        }
                        else
                        {
                            running = false;
                        }
                    }
                    break;
                case SDLK_f: if(!down) { set_input_fps(!input_fps); } break;
                case SDLK_w: input_w = down; break;
                case SDLK_a: input_a = down; break;
                case SDLK_s: input_s = down; break;
                case SDLK_d: input_d = down; break;
                case SDLK_SPACE: input_space = down; break;
                case SDLK_LCTRL: case SDLK_RCTRL: input_ctrl = down; break;
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

        spot_light.position = camera_position;
        spot_light.direction = camera_front;

        // handle mouse input
        if(input_fps)
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
        if(input_fps)
        {
            auto camera_movement = glm::vec3{0.0f, 0.0f, 0.0f};
            auto camera_movement_requested = false;
            const auto move_camera = [&](const glm::vec3& m, bool p, bool n)
            {
                int d = 0;
                if(p) d += 1;
                if(n) d -= 1;
                if(d != 0)
                {
                    camera_movement += m * static_cast<float>(d);
                    camera_movement_requested = true;
                }
            };
            move_camera(camera_front, input_w, input_s);
            move_camera(camera_right, input_d, input_a);
            move_camera(camera_up, input_space, input_ctrl);

            if(camera_movement_requested)
            {
                const auto camera_speed = 3 * dt * (input_shift ? 2.0f : 1.0f);
                camera_position += camera_speed * glm::normalize(camera_movement);
            }
        }

        if(animate)
        {
            time += dt;

            if(time > 2*pi)
            {
                time -= 2* pi;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        {
            const auto view = glm::lookAt(camera_position, camera_position + camera_front, UP);

            const auto pv = projection * view;

            for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
            {
                light_shader.Use();
                light_shader.SetVec3(uni_light_color, point_lights[i].diffuse);
                {
                    const auto model = glm::translate(glm::mat4(1.0f), point_lights[i].position);
                    light_shader.SetMat(uni_light_transform, pv * model);
                }
                light_mesh.Draw();
            }

            shader.Use();
            shader.SetVec4(uni_color, cube_color);
            uni_material.SetShader(&shader, material);
            uni_directional_light.SetShader(&shader, directional_light);
            uni_spot_light.SetShader(&shader, spot_light);
            for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
            {
                uni_point_lights[i].SetShader(&shader, point_lights[i]);
            }
            shader.SetVec3(uni_view_position, camera_position);
            
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
                    shader.SetMat(uni_transform, pv * model);
                    shader.SetMat(uni_model_transform, model);
                    shader.SetMat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                }
                mesh.Draw();
            }

            {
                const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
                shader.SetMat(uni_transform, pv * model);
                shader.SetMat(uni_model_transform, model);
                shader.SetMat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                plane_mesh.Draw();
            }
        }

        if(input_fps == false)
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
                    set_input_fps(true);
                }
                ImGui::Checkbox("Animate?", &animate);
                ImGui::SameLine();
                ImGui::SliderFloat("Time", &time, 0.0f, 2 * pi);
                if(ImGui::Combo("Rendering mode", &rendering_mode, "Fill\0Line\0Point\0"))
                {
                    set_rendering_mode();
                }
                if(ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 145.0f))
                {
                    update_viewport();
                }
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

            PrintProfiles();

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
