#include "tred/fyro.h"
#include "tred/rect.h"
#include "tred/viewportdef.h"

#include "cards.png.h"
#include "sprites/cards.h"

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

void set_gl_viewport(const recti& r)
{
    glViewport(r.minx, r.miny + r.get_height(), r.get_width(), r.get_height());
}

struct Layer2
{
    SpriteBatch* batch;

    ~Layer2()
    {
        batch->submit();
    }
};

Layer2 create_layer(const RenderCommand2& rc, const ViewportDef& vp, const glm::mat4 camera)
{
    set_gl_viewport(vp.screen_rect);

    const auto projection = glm::ortho(0.0f, vp.virtual_width, 0.0f, vp.virtual_height);

    rc.render->quad_shader.Use();
    rc.render->quad_shader.SetMat(rc.render->view_projection_uniform, projection);
    rc.render->quad_shader.SetMat(rc.render->transform_uniform, camera);

    return Layer2{&rc.render->batch};
}

Layer2 with_layer_fit_with_bars(const RenderCommand2& rc, float requested_width, float requested_height, const glm::mat4 camera)
{
    const auto vp = ViewportDef::FitWithBlackBars(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(rc, vp, camera);
}

Layer2 with_layer_extended(const RenderCommand2& rc, float requested_width, float requested_height, const glm::mat4 camera)
{
    const auto vp = ViewportDef::Extend(requested_width, requested_height, rc.size.x, rc.size.y);
    return create_layer(rc, vp, camera);
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
    }

    void
    OnRender(const RenderCommand2& rc) override
    {
        // auto r = with_layer_fit_with_bars(rc, 200.0f, 200.0f, glm::mat4(1.0f));
        auto r = with_layer_extended(rc, 200.0f, 200.0f, glm::mat4(1.0f));

        // todo(Gustav): change to a full window rect bc clear doesn't respect viewport
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        r.batch->quad(&cards, ::cards::hearts[2].zero().set_height(30).translate(100, 100), get_sprite(cards, ::cards::hearts[2]));
        r.batch->quad(&cards, ::cards::back.zero().set_height(30).translate(10, 50), get_sprite(cards, ::cards::back));
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
