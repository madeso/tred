#include "tred/fyro.h"
#include "tred/rect.h"

#include "cards.png.h"
#include "sprites/cards.h"

struct SpriteBatch
{
    std::vector<float> data;
    int quads = 0;
    int max_quads = 100;

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

    void submit()
    {
        if(quads == 0)
        {
            return;
        }

        data.resize(0);
        quads = 0;
    }
};

// todo(Gustav): figure out uv layout... what is up/down? what is left/right
// what do we want? like euph? probably...
rect get_sprite(const Texture& texture, const rect& r)
{
    const auto w = 1.0f/static_cast<float>(texture.width);
    const auto h = 1.0f/static_cast<float>(texture.height);
    return {r.minx * w, 1-r.maxy * h, r.maxx * w, 1-r.miny * h};
}

void label(const char* label, const std::string& text)
{
    ImGui::LabelText(label, "%s", text.c_str());
}

void selectable(const std::string& text)
{
    bool selected = false;
    ImGui::Selectable(text.c_str(), &selected);
}

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

        constexpr auto max_quads = 100;

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

    GLuint va;
    GLuint vb;
    GLuint ib;

    SpriteBatch batch = SpriteBatch{};

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

        batch.quad(rect{-0.5f, -0.5f, 0.5f, 0.5f}, get_sprite(cards, ::cards::hearts[2]));
        const auto d = 0.5f;
        batch.quad(rect{-0.5f+d, -0.5f + d, 0.5f + d, 0.5f + d}, get_sprite(cards, ::cards::back));

        glBindVertexArray(va);
        glBufferSubData
        (
            GL_ARRAY_BUFFER,
            0,
            static_cast<GLsizeiptr>(sizeof(float) * batch.data.size()),
            static_cast<const void*>(batch.data.data())
        );
        glDrawElements(GL_TRIANGLES, 6 * batch.quads, GL_UNSIGNED_INT, nullptr);
        batch.submit();
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

