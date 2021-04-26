#include "tred/fyro.h"

#include "cards.png.h"
#include "letter_g.png.h"
#include "sprites/cards.h"

struct ExampleGame : public Game
{
    Texture cards;
    Texture letter_g;

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
        , letter_g
        (
            LoadImageEmbeded
            (
                LETTER_G_PNG,
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

        r.batch->quad({}, r.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});

        constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(30);

        r.batch->quad(&cards, card_sprite.translate(100, 100), ::cards::hearts[2]);
        r.batch->quad(&cards, card_sprite.translate(10, 50), ::cards::back);

        r.batch->quad(&letter_g, rect{40, 40}.translate(50, 50), {});
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
