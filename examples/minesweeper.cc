#include "tred/fyro.h"

#include <array>
#include <vector>

#include "onebit.png.h"
#include "sprites/onebit.h"

#include "tred/random.h"

constexpr auto font_size = 12.0f;

struct text_animation
{
    virtual ~text_animation() = default;
    virtual rect transform(int at, const rect& r) const = 0;
};

struct no_text_animation : public text_animation
{
    rect transform(int, const rect& r) const override { return r; }
};

struct siny_animation : public text_animation
{
    float change;
    float scale;
    float offset;

    siny_animation(float c, float s, float o)
        : change(c)
        , scale(s)
        , offset(o)
    {
    }

    rect transform(int index, const rect& r) const override
    {
        const auto a = static_cast<float>(index) * scale + offset;
        constexpr float one_turn = 6.28318530718f; // 2*pi, thanks google
        return r.translate(0.0f, std::sin(a * one_turn) * change);
    }
};

constexpr auto font_spacing = font_size * 0.7f;

void simple_text(sprite_batch* batch, texture* onebit, const glm::vec4 color, float x, float y, const std::string& text, const text_animation& anim)
{
    constexpr auto sprite = rect{font_size, font_size};


    int position_in_string = 0;

    for(char c: text)
    {
        if(c == ' ')
        {
            x += font_spacing;
            position_in_string += 1;
        }
        else
        {
            // silently ignore missing characters
            if(auto index = ::onebit::text_string.find(c); index != std::string_view::npos)
            {
                batch->quad(onebit, anim.transform(position_in_string, sprite.translate(x, y)), ::onebit::text[static_cast<std::size_t>(index)], color);
                x += font_spacing;
                position_in_string += 1;
            }
        }
    }
}

float get_width_of_string(const std::string& text)
{
    return static_cast<float>(text.length()) * font_spacing;
}

// todo(Gustav): add markers.. bomb and question
// todo(Gustav): add bombs counter
// todo(Gustav): wiggle things a bit
// todo(Gustav): animate floodfill
// todo(Gustav): add some colors
// todo(Gustav): center board
// todo(Gustav): ui for selecting level size and number of bombs


constexpr auto black = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};

enum class game_state { playing, game_over, game_completed};

struct minesweeper
{
    int width;
    int height;
    int bombs;
    std::vector<bool> revealed;
    // todo(Gustav): never used font for 1+ rendering... so switch to a enum class
    std::vector<int> states; // -1 == bomb, 0=empty, 1-9=number of neighbour bombs
    bool initialized = false;
    game_state current_game_state = game_state::playing;

    rect play_area = {100.0f, 100.0f};

    minesweeper(int w, int h, int b)
        : width(w), height(h), bombs(b)
        , revealed(Cint_to_sizet(w*h), false)
        , states(Cint_to_sizet(w*h), 0)
    {
    }

    void init()
    {
        if(initialized) { return; }
        initialized = true;

        using free_type = std::vector<glm::ivec2>;
        free_type free;
        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                if(false == revealed[get_index(x, y)])
                {
                    free.emplace_back(glm::ivec2{x, y});
                }
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

        init();

        if(states[get_index(x, y)] == -1)
        {
            game_over();
        }

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

    void game_over()
    {
        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<width; x+=1)
            {
                revealed[get_index(x, y)] = true;
            }
        }
        current_game_state = game_state::game_over;
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
        assert(x >= 0 && x<width);
        assert(y >= 0 && y<width);
        return Cint_to_sizet(x + y * height);
    }

    static constexpr float window_size = 200.0f;
    static constexpr float window_spacing = 10.0f;

    rect sprite() const
    {
        const auto w = play_area.get_width()/static_cast<float>(width);
        const auto h = play_area.get_height()/static_cast<float>(height);
        const auto s = std::min(w, h);
        return {s, s};
    }

    rect get_rect(int x, int y) const
    {
        const auto center = [](float sprite_size, float avaiable, int count) -> float
        {
            return (avaiable - sprite_size * static_cast<float>(count)) / 2.0f;
        };
        const auto s = sprite();
        const auto base_x = play_area.minx + center(s.get_width(), play_area.get_width(), width);
        const auto base_y = play_area.miny + center(s.get_height(), play_area.get_height(), height);
        return s.translate
        (
            base_x + static_cast<float>(x) * s.get_width(),
            base_y + static_cast<float>(y) * s.get_height()
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
        new_game();
    }

    void new_game()
    {
        ms = minesweeper{20, 10, 10};
    }

    layout_data get_main_layout() const
    {
        // return {viewport_style::black_bars, 200.0f, 200.0f, glm::mat4(1.0f)};
        return {viewport_style::extended, 200.0f, 200.0f, glm::mat4(1.0f)};
    }

    float title_anim = 0.0f;
    bool on_update(float dt) override
    {
        title_anim += dt * 1.0f;
        while(title_anim > 1.0f) { title_anim -= 1.0f;}
        return true;
    }

    void
    on_render(const render_command2& rc) override
    {
        const std::string game_title = "minesweeper 42";

        constexpr float spacing = 0.2f;
        constexpr float button_size = 15.0f;
        constexpr float wavy_range = 1.5f;
        auto r = with_layer(rc, get_main_layout());

        auto game_world = rect{200.0f, 200.0f};

        const auto title_rect = cut_bottom(&game_world, font_size + wavy_range + spacing * 2)
            .center_hor(get_width_of_string(game_title))
            .center_vert(font_size);
        const auto restart_rect = cut_bottom(&game_world, button_size + spacing * 2);
        /* const auto score_rect = */ cut_bottom(&game_world, font_size + spacing * 2);
        ms.play_area = game_world.extend(-1.0f);
        game_button = restart_rect.cut_to_center(button_size, button_size);

        r.batch->quad({}, r.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});

        ms.render(r.batch, &onebit);

        auto draw_game_button = [&](const recti& sprite)
        {
            r.batch->quad(&onebit, game_button, sprite, black);
        };

        switch(ms.current_game_state)
        {
            case game_state::playing: draw_game_button(::onebit::smiley_happy); break;
            case game_state::game_completed: draw_game_button(::onebit::smiley_joy); break;
            case game_state::game_over: draw_game_button(::onebit::smiley_skull); break;
        }

        simple_text(r.batch, &onebit, black, title_rect.minx, title_rect.miny, game_title, siny_animation{wavy_range, 0.2f, title_anim});
    }

    void on_mouse_position(const glm::ivec2& p) override
    {
        mouse = {p.x, p.y};
    }

    rect game_button = rect{15.0f, 15.0f}.translate(100, 160);

    void on_mouse_button(const command2& c, input::mouse_button button, bool down) override
    {
        if(down != false) { return; }
        if(button != input::mouse_button::left) { return; }

        auto r = with_layer(c, get_main_layout());
        const auto world = r.mouse_to_world(mouse);
        ms.on_mouse(world);

        if(game_button.is_inside_inclusive(world.x, world.y))
        {
            new_game();
        }
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
