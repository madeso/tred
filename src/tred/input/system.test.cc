#include "tred/handle.h"

#include <sstream>
#include <functional>
#include "fmt/format.h"

#include "catch.hpp"

#include "tred/input/system.h"
#include "tred/input/player.h"
#include "tred/input/config.h"
#include "tred/input/table.h"

#include "catchy/mapeq.h"

using namespace input;
using namespace Catch::Matchers;
using namespace catchy;


namespace
{


struct TestPlatform : public Platform
{
    std::map<JoystickId, std::string> joysticks;

    // todo(Gustav): rename to GetAvailableJoysticks
    std::vector<JoystickId> ActiveAndFreeJoysticks() override
    {
        auto r = std::vector<JoystickId>{};
        for(const auto& i: joysticks)
        {
            r.emplace_back(i.first);
        }
        return r;
    }

    bool MatchUnit(JoystickId id, const std::string& guid) override
    {
        auto found = joysticks.find(id);
        if(found == joysticks.end())
        {
            return false;
        }
        return found->second == guid;
    }

    void StartUsing(JoystickId joy) override
    {
        joysticks.erase(joy);
    }
};

using Data = decltype(Table::data);

Data GetTable(InputSystem* system, PlayerHandle player, float dt = 1.0f)
{
    Table table;
    system->UpdateTable(player, &table, dt);
    return table.data;
}


FalseString MapEq(const std::map<std::string, float>& lhs, const std::map<std::string, float>& rhs)
{
    return catchy::MapEq(lhs, rhs, [](float l, float r) -> FalseString
    {
        const auto eps = std::numeric_limits<float>::epsilon() * 100;
        const bool same = std::abs(r-l) < eps;
        if(same) return FalseString::True();
        return FalseString::False(fmt::format("{} != {}", l, r));
    });
}


}


TEST_CASE("input-test", "[input]")
{
    constexpr auto JOYSTICK_HANDLE = static_cast<JoystickId>(42);
    constexpr int JOYSTICK_START = 0;
    constexpr int JOYSTICK_SHOOT = 1;
    const std::string JOYSTICK_GUID = "joystick-guid";
    constexpr int JOYSTICK_MOVE = 0;
    constexpr int JOYSTICK_LOOK = 1;

    auto sys = InputSystem
    {
        {
            // todo(Gustav): add groupings... (game: use this group now) car/walk/swim ...
            // todo(Gustav): add categories/tags (better name) (input: use theese functions now): point selection, grid selection...
            // todo(Gustav): support virtual keys/axis for touch controls like 'sinput'
            // todo(Gustav): support button images for help text and display in config files
            // todo(Gustav): add 2 player tests (both joystick only and keyboard+joystick to test assignment blocking)
            // todo(Gustav): add multiple key bindings

            // todo(Gustav): add bad config tests
            // todo(Gustav): add invert axis bools
            // todo(Gustav): add axis sensitivity
            // todo(Gustav): add axis scales functions
            // todo(Gustav): add haptic feedback
            // todo(Gustav): add joystick smoothing for use in sdl implementation or global
            {
                {"shoot", "var_shoot", Range::WithinZeroToOne},
                {"look", "var_look", Range::Infinite},
                {"move", "var_move", Range::WithinNegativeOneToPositiveOne},
            },

            // controller setup (bind)
            {
                {
                    "mouse+keyboard",
                    {
                        config::KeyboardDef{Key::RETURN},
                        config::MouseDef{MouseButton::LEFT}
                    },
                    {
                        config::KeyBindDef{"shoot", 0, Key::A},
                        config::TwoKeyBindDef{"move", 0, Key::LEFT, Key::RIGHT},
                        config::AxisBindDef{"look", 1, Axis::X}
                    }
                },
                {
                    "joystick",
                    {
                        config::JoystickDef{JOYSTICK_START, JOYSTICK_GUID}
                    },
                    {
                        config::KeyBindDef{"shoot", 0, JOYSTICK_SHOOT},
                        config::AxisBindDef{"move", 0, AxisType::GeneralAxis, 0, JOYSTICK_MOVE},
                        config::AxisBindDef{"look", 0, AxisType::GeneralAxis, 0, JOYSTICK_LOOK}
                    }
                }
            }
        }
    };

    auto test_platform = TestPlatform{};

    auto player = sys.AddPlayer();

    SECTION("no assigned control is valid")
    {
        REQUIRE(MapEq(GetTable(&sys, player), {
        }));
    }

    SECTION("test auto assignments")
    {
        REQUIRE_FALSE(sys.IsConnected(player));

        auto update = [&]() { sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform); };
        update();

        REQUIRE(sys.IsConnected(player));

        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        test_platform.joysticks[JOYSTICK_HANDLE] = JOYSTICK_GUID;
        REQUIRE(test_platform.joysticks.size() == 1);

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);
        update();

        // joystick was grabbed
        REQUIRE(test_platform.joysticks.size() == 0);

        // player is still connected
        REQUIRE(sys.IsConnected(player));


        // keyboard input is ignored...

        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // but joystick works...

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // loose joystick...
        const auto joystickbutton_was_down_when_lost = GENERATE(false, true);
        INFO("joystickbutton_was_down_when_lost: " << joystickbutton_was_down_when_lost);
        if(joystickbutton_was_down_when_lost)
        {
            sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        }
        sys.OnJoystickLost(JOYSTICK_HANDLE);
        update();

        // no input
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // joystick should be ignored
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);

        // player is still connected
        REQUIRE(sys.IsConnected(player));

        // and keyboard works
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("test explicit assignments keyboard keyboard+mouse")
    {
        const auto use_keyboard = GENERATE(true, false);
        INFO("Use keyboard " << use_keyboard);
        auto press = [&](bool down)
        {
            if(use_keyboard) { sys.OnKeyboardKey(Key::RETURN, down); }
            else { sys.OnMouseButton(MouseButton::LEFT, down); }
        };

        REQUIRE_FALSE(sys.IsConnected(player));

        auto update = [&]() { sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform); };
        update();

        REQUIRE_FALSE(sys.IsConnected(player));

        press(true);
        update();

        REQUIRE_FALSE(sys.IsConnected(player));

        press(false);
        update();

        REQUIRE(sys.IsConnected(player));

        // keyboard input works
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // add a joystick
        test_platform.joysticks[JOYSTICK_HANDLE] = JOYSTICK_GUID;
        REQUIRE(test_platform.joysticks.size() == 1);

        // press start
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);

        // but it is not picked up
        update();
        REQUIRE(test_platform.joysticks.size() == 1);

        // and joystick is ignored
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // but input is keyboard+mouse again
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("test explicit assignments joystick")
    {
        // add a joystick
        test_platform.joysticks[JOYSTICK_HANDLE] = JOYSTICK_GUID;
        REQUIRE(test_platform.joysticks.size() == 1);

        const auto use_keyboard = GENERATE(true, false);
        INFO("Use keyboard " << use_keyboard);
        auto press = [&](bool down)
        {
            if(use_keyboard) { sys.OnKeyboardKey(Key::RETURN, down); }
            else { sys.OnMouseButton(MouseButton::LEFT, down); }
        };

        REQUIRE_FALSE(sys.IsConnected(player));

        auto update = [&]() { sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform); };
        update();

        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        update();

        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);
        update();

        REQUIRE(sys.IsConnected(player));
        REQUIRE(test_platform.joysticks.size() == 0);

        // keyboard input works
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // press start
        press(true);
        press(false);

        // update but it shouldn't be picked up
        update();

        // and keyboard is ignored
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // loose joystick...
        const auto joystickbutton_was_down_when_lost = GENERATE(false, true);
        INFO("joystickbutton_was_down_when_lost: " << joystickbutton_was_down_when_lost);
        if(joystickbutton_was_down_when_lost)
        {
            sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        }
        sys.OnJoystickLost(JOYSTICK_HANDLE);
        update();

        REQUIRE_FALSE(sys.IsConnected(player));

        // no input
        REQUIRE(MapEq(GetTable(&sys, player), {
        }));

        // joystick should be ignored
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player), {
        }));
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);
    }

    // now... for the rest of the tests select the player input ("mouse and keyboard")
    sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

    auto use_joystick_now = [&]()
    {
        test_platform.joysticks[JOYSTICK_HANDLE] = JOYSTICK_GUID;
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);
    };

    SECTION("no input")
    {
        REQUIRE(MapEq(GetTable(&sys, player), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("push button")
    {
        const auto ignore_dt = GENERATE(0.5f, 1.0f);

        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player, ignore_dt), {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player, ignore_dt), {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("look with")
    {
        const auto ignore_dt = GENERATE(0.5f, 1.0f);

        SECTION("joystick")
        {
            use_joystick_now();

            sys.OnJoystickAxis(JOYSTICK_HANDLE, JOYSTICK_LOOK, 1.0f);
            REQUIRE(MapEq(GetTable(&sys, player, 1.0f), {
                {"var_shoot", 0.0f},
                {"var_look", 1.0f},
                {"var_move", 0.0f}
            }));
            REQUIRE(MapEq(GetTable(&sys, player, 0.5f), {
                {"var_shoot", 0.0f},
                {"var_look", 0.5f},
                {"var_move", 0.0f}
            }));

            sys.OnJoystickAxis(JOYSTICK_HANDLE, JOYSTICK_LOOK, 0.0f);
            REQUIRE(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));
        }

        SECTION("mouse")
        {
            sys.OnMouseAxis(Axis::X, 2.0f, 0.0f);
            REQUIRE(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 2.0f},
                {"var_move", 0.0f}
            }));

            sys.OnMouseAxis(Axis::X, 0.0f, 0.0f);
            REQUIRE(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));
        }
    }

    SECTION("move with")
    {
        const auto ignore_dt = GENERATE(0.5f, 1.0f);

        SECTION("joystick")
        {
            use_joystick_now();

            // start clean
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));

            sys.OnJoystickAxis(JOYSTICK_HANDLE, JOYSTICK_MOVE, -1.0f);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", -1.0f}
            }));

            sys.OnJoystickAxis(JOYSTICK_HANDLE, JOYSTICK_MOVE, 0.0f);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));

            // only right down
            sys.OnJoystickAxis(JOYSTICK_HANDLE, JOYSTICK_MOVE, 1.0f);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 1.0f}
            }));
        }

        SECTION("keyboard")
        {
            // start clean
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));

            // left down
            sys.OnKeyboardKey(Key::LEFT, true);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", -1.0f}
            }));

            // left and right down
            sys.OnKeyboardKey(Key::RIGHT, true);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));

            // only right down
            sys.OnKeyboardKey(Key::LEFT, false);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 1.0f}
            }));

            // nothing down
            sys.OnKeyboardKey(Key::RIGHT, false);
            CHECK(MapEq(GetTable(&sys, player, ignore_dt), {
                {"var_shoot", 0.0f},
                {"var_look", 0.0f},
                {"var_move", 0.0f}
            }));
        }
    }
}

