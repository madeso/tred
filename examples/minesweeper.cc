#include "tred/fyro.h"

#include <array>
#include <vector>

#include "onebit.png.h"
#include "sprites/onebit.h"

#include "tred/random.h"

constexpr auto font_size = 12.0f;

void simple_text(sprite_batch* batch, texture* onebit, const glm::vec4 color, float x, float y, const std::string& text)
{
    constexpr auto spacing = font_size * 0.7f;
    constexpr auto sprite = rect{font_size, font_size};

    for(char c: text)
    {
        if(c == ' ')
        {
            x += spacing;
        }
        else
        {
            // silently ignore missing characters
            if(auto index = ::onebit::text_string.find(c); index != std::string_view::npos)
            {
                batch->quad(onebit, sprite.translate(x, y), ::onebit::text[static_cast<std::size_t>(index)], color);
                x += spacing;
            }
        }
    }
}

constexpr auto sprite = rect{15.0f, 15.0f};
constexpr auto black = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
constexpr float base_x = 0.0f;
constexpr float base_y = 0.0f;

struct minesweeper
{
    int width;
    int height;
    std::vector<bool> revealed;
    std::vector<int> states; // -1 == bomb, 0=empty, 1-9=number of neighbour bombs

    minesweeper(int w, int h, int bombs)
        : width(w), height(h)
        , revealed(Cint_to_sizet(w*h), false)
        , states(Cint_to_sizet(w*h), 0)
    {
        using free_type = std::vector<glm::ivec2>;
        free_type free;
        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                free.emplace_back(glm::ivec2{x, y});
            }
        }
        Random r;
        for(int i=0; i<bombs; i+=1)
        {
            const auto index = r.get_excluding(free.size());
            const auto p = free[index];
            free.erase(free.begin() + static_cast<free_type::difference_type>(index));
            states[get_index(p.x, p.y)] = -1;
        }

        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                if(states[get_index(x, y)]==0)
                {
                    states[get_index(x, y)] = count_neighbours(x, y);
                }
            }
        }
    }

    void click_on(int x, int y)
    {
        revealed[get_index(x, y)] = true;

        // auto-click on empties
        if(states[get_index(x, y)] == 0)
        {
            for(int dx=-1; dx<=1; dx+=1)
            {
                for(int dy=-1; dy<=1; dy+=1)
                {
                    const auto xx = x + dx;
                    const auto yy = y + dy;
                    if(xx <0 || yy < 0 || xx >= width || yy >= height) { continue; }
                    if(false == revealed[get_index(xx, yy)])
                    {
                        click_on(xx, yy);
                    }
                }
            }
        }
    }

    void on_mouse(const glm::vec2& m)
    {
        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                if(get_rect(x, y).is_inside_inclusive(m.x, m.y))
                {
                    click_on(x, y);
                }
            }
        }
    }

    size_t get_index(int x, int y) const
    {
        return Cint_to_sizet(x + y * height);
    }

    rect get_rect(int x, int y) const
    {
        return sprite.translate
        (
            base_x + static_cast<float>(x) * sprite.get_width(),
            base_y + static_cast<float>(y) * sprite.get_height()
        );
    }

    int count_neighbours(int x, int y) const
    {
        int count = 0;
        for(int dx=-1; dx<=1; dx+=1)
        {
            for(int dy=-1; dy<=1; dy+=1)
            {
                if(dx == 0 && dy == 0) { continue; }
                const auto xx = x + dx;
                const auto yy = y + dy;
                if(xx <0 || yy < 0 || xx >= width || yy >= height) { continue; }
                if(states[get_index(xx, yy)] == -1)
                {
                    count += 1;
                }
            }
        }

        return count;
    }

    void render(sprite_batch* batch, texture* onebit)
    {
        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                auto draw = [x, y, batch, onebit, this](const recti& sprite, const glm::vec4& color)
                {
                    batch->quad
                    (
                        onebit,
                        get_rect(x, y),
                        sprite, color
                    );
                };

                const auto index = get_index(x, y);
                const auto is_revealed = revealed[index];
                const auto state = states[index];
                if(is_revealed && state == 0) { continue; }

                constexpr auto color = black;

                if(is_revealed == false)
                {
                    draw(::onebit::box, color);
                }
                else
                {
                    switch(state)
                    {
                    case -1: draw(::onebit::bomb, color); break;
                    case 1:  draw(::onebit::n1, color); break;
                    case 2:  draw(::onebit::n2, color); break;
                    case 3:  draw(::onebit::n3, color); break;
                    case 4:  draw(::onebit::n4, color); break;
                    case 5:  draw(::onebit::n5, color); break;
                    case 6:  draw(::onebit::n6, color); break;
                    case 7:  draw(::onebit::n7, color); break;
                    case 8:  draw(::onebit::n8, color); break;
                    case 9:  draw(::onebit::n9, color); break;
                    }
                }
            }
        }
    }
};

struct minesweeper_game : public game
{
    texture onebit;
    glm::vec2 mouse;

    minesweeper ms;

    minesweeper_game()
        : onebit
        (
            load_image_from_embedded
            (
                ONEBIT_PNG,
                texture_edge::clamp,
                texture_render_style::pixel,
                transparency::include
            )
        )
        , mouse(0, 0)
        , ms(10, 10, 10)
    {
    }

    void
    on_render(const render_command2& rc) override
    {
        // auto r = with_layer_fit_with_bars(rc, 200.0f, 200.0f, glm::mat4(1.0f));
        auto r = with_layer_extended(rc, 200.0f, 200.0f, glm::mat4(1.0f));

        r.batch->quad({}, r.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});

        ms.render(r.batch, &onebit);

        simple_text(r.batch, &onebit, black, 0.0f, 180.0f, "minesweeper 42");
    }

    void on_mouse_position(const glm::ivec2& p) override
    {
        mouse = {p.x, p.y};
    }

    void on_mouse_button(const command2& c, input::mouse_button button, bool down) override
    {
        if(down != false) { return; }
        if(button != input::mouse_button::left) { return; }

        auto r = with_layer_extended(c, 200.0f, 200.0f, glm::mat4(1.0f));
        const auto world = r.mouse_to_world(mouse);
        ms.on_mouse(world);
    }
};

int
main(int, char**)
{
    return run_game
    (
        "minesweeper", glm::ivec2{800, 600}, false, []()
        {
            return std::make_shared<minesweeper_game>();
        }
    );
}
