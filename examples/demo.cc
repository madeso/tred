#include "tred/fyro.h"
#include "tred/rect.h"

#include "cards.png.h"

struct SpriteBatch
{
    void add_vertex(const glm::vec2& position, const glm::vec4& color, const glm::vec2& uv)
    {
        add_vertex({position.x, position.y, 0.0f}, color, uv);
    }

    void add_vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv)
    {
        data.push_back(position.x);
        data.push_back(position.y);
        data.push_back(position.z);

        data.push_back(color.x);
        data.push_back(color.y);
        data.push_back(color.z);
        data.push_back(color.w);

        data.push_back(uv.x);
        data.push_back(uv.y);
    }

    void quad(const rect& scr, const rect& tex, const glm::vec4& tint = glm::vec4(1.0f))
    {
        quads += 1;

        add_vertex({scr.minx, scr.miny}, tint, {tex.minx, tex.miny});
        add_vertex({scr.maxx, scr.miny}, tint, {tex.maxx, tex.miny});
        add_vertex({scr.maxx, scr.maxy}, tint, {tex.maxx, tex.maxy});
        add_vertex({scr.minx, scr.maxy}, tint, {tex.minx, tex.maxy});
    }

    void clear()
    {
        data.resize(0);
        quads = 0;
    }

    std::vector<float> data;
    int quads = 0;
};

struct ExampleGame : public Game
{
    Texture cards;

    ExampleGame()
        : cards
        (
            LoadImageEmbeded
            (
                CARDS_PNG,
                TextureEdge::Clamp,
                TextureRenderStyle::Pixel,
                Transperency::Include
            )
        )
    {
        quad_shader.Use();

        glGenVertexArrays(1, &va);
        glBindVertexArray(va);

        constexpr auto vertex_size = 9 * sizeof(float);
        constexpr auto max_vertices = 100;

        glGenBuffers(1, &vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(0 * sizeof(float)));
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, reinterpret_cast<void*>(7 * sizeof(float)));

        u32 indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        glGenBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    GLuint va;
    GLuint vb;
    GLuint ib;

    void
    OnRender(const glm::ivec2& size) override
    {
        glViewport(0, 0, size.x, size.y);
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        const auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

        quad_shader.Use();
        quad_shader.SetMat(view_projection_uniform, projection);
        quad_shader.SetMat(transform_uniform, glm::mat4(1.0f));

        BindTexture(texture_uniform, cards);

        auto batch = SpriteBatch{};
        batch.quad(rect{-0.5f, -0.5f, 0.5f, 0.5f}, rect{1.0f, 1.0f});

        glBindVertexArray(va);
        glBufferSubData
        (
            GL_ARRAY_BUFFER,
            0,
            static_cast<GLsizeiptr>(sizeof(float) * batch.data.size()),
            static_cast<const void*>(batch.data.data())
        );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};

int
main(int, char**)
{
    return Run
    (
        "Example", glm::ivec2{800, 600}, false, []()
        {
            return std::make_shared<ExampleGame>();
        }
    );
}

