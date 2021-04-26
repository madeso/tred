#include "tred/fyro.h"

#include <array>
#include <random>

#include "cards.png.h"
#include "letter_g.png.h"
#include "sprites/cards.h"

constexpr std::array<int, 64> endesga64 =
{
    0xff0040, 0x131313, 0x1b1b1b, 0x272727, 0x3d3d3d, 0x5d5d5d, 0x858585, 0xb4b4b4,
    0xffffff, 0xc7cfdd, 0x92a1b9, 0x657392, 0x424c6e, 0x2a2f4e, 0x1a1932, 0x0e071b,
    0x1c121c, 0x391f21, 0x5d2c28, 0x8a4836, 0xbf6f4a, 0xe69c69, 0xf6ca9f, 0xf9e6cf,
    0xedab50, 0xe07438, 0xc64524, 0x8e251d, 0xff5000, 0xed7614, 0xffa214, 0xffc825,
    0xffeb57, 0xd3fc7e, 0x99e65f, 0x5ac54f, 0x33984b, 0x1e6f50, 0x134c4c, 0x0c2e44,
    0x00396d, 0x0069aa, 0x0098dc, 0x00cdf9, 0x0cf1ff, 0x94fdff, 0xfdd2ed, 0xf389f5,
    0xdb3ffd, 0x7a09fa, 0x3003d9, 0x0c0293, 0x03193f, 0x3b1443, 0x622461, 0x93388f,
    0xca52c9, 0xc85086, 0xf68187, 0xf5555d, 0xea323c, 0xc42430, 0x891e2b, 0x571c27
};

struct color_flips
{
    static constexpr int width = 100;
    static constexpr int height = 100;
    static constexpr float update_time = 15.0f;
    static constexpr double max_time = 15.0;
    static constexpr float size = 1.7f;
    static constexpr float spacing = 2.0f;
    static constexpr float startx = 0.0f;
    static constexpr float starty = 0.0f;

    float timer[width * height];
    std::size_t index[width*height];

    color_flips()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> time_random(0.0, max_time);
        std::uniform_int_distribution<> index_random(0, endesga64.size());

        for(size_t i=0; i<width*height; i+=1) { timer[i] = static_cast<float>(time_random(gen)); }
        for(size_t i=0; i<width*height; i+=1) { index[i] = static_cast<std::size_t>(index_random(gen)); }
    }

    void update(float dt)
    {
        for(size_t i=0; i<width*height; i+=1)
        {
            timer[i] -= dt;
            if(timer[i] < 0.0f)
            {
                timer[i] += update_time;
                index[i] = (index[i] + 1) % endesga64.size();
            }
        }
    }

    void render(SpriteBatch* batch)
    {
        const auto color = [](int c, int s) -> float
        {
            return ((c >> s) & 0xff) / 255.0f;
        };

        for(size_t x=0; x<width; x+=1)
        for(size_t y=0; y<height; y+=1)
        {
            const auto c = endesga64[index[x*width + y]];
            batch->quad
            (
                {},
                rect{size, size}
                    .translate(startx, starty)
                    .translate(static_cast<float>(x)*spacing, static_cast<float>(y)*spacing),
                {}, {color(c, 2), color(c, 1), color(c, 0), 1.0f});
        }
    }
};

struct ExampleGame : public Game
{
    Texture cards;
    Texture letter_g;
    color_flips flips;

    ExampleGame()
        : cards
        (
            LoadImageEmbeded
            (
                CARDS_PNG,
                TextureEdge::Clamp,
                TextureRenderStyle::Smooth,
                Transperency::Include
            )
        )
        , letter_g
        (
            LoadImageEmbeded
            (
                LETTER_G_PNG,
                TextureEdge::Clamp,
                TextureRenderStyle::Smooth,
                Transperency::Include
            )
        )
    {
    }

    bool
    OnUpdate(float dt) override
    {
        flips.update(dt);
        return true;
    }

    void
    OnRender(const RenderCommand2& rc) override
    {
        // auto r = with_layer_fit_with_bars(rc, 200.0f, 200.0f, glm::mat4(1.0f));
        auto r = with_layer_extended(rc, 200.0f, 200.0f, glm::mat4(1.0f));

        r.batch->quad({}, r.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});

        flips.render(r.batch);

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
