#include "tred/fyro.h"

#include "cards.png.h"
#include "sprites/cards.h"

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

        // todo(Gustav): figure out uv layout... what is up/down? what is left/right
        // what do we want? like euph? probably...
        // render the G sprite to test orientation

        r.batch->quad({}, r.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});

        constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(30);

        r.batch->quad(&cards, card_sprite.translate(100, 100), ::cards::hearts[2]);
        r.batch->quad(&cards, card_sprite.translate(10, 50), ::cards::back);
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
