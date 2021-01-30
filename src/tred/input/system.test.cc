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
#include "catchy/stringeq.h"

using namespace input;
using namespace Catch::Matchers;
using namespace catchy;


// todo(Gustav): add groupings... (game: use this group now) car/walk/swim ...
// todo(Gustav): add categories/tags (better name) (input: use theese functions now): point selection, grid selection...
// todo(Gustav): support virtual keys/axis for touch controls like 'sinput'
// todo(Gustav): support button images for help text and display in config files
// todo(Gustav): add multiple key bindings
// todo(Gustav): add support for runtime rebinding

// todo(Gustav): add axis scales functions
// todo(Gustav): add haptic feedback
// todo(Gustav): add 2button sensitvity for relative movement (time based?)
// todo(Gustav): add joystick smoothing for use in sdl implementation or global
// todo(Gustav): add 2 player tests (both joystick only and keyboard+joystick to test assignment blocking)
// todo(Gustav): add sdl gamepad
// todo(Gustav): add keyboard input


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

    void OnChangedConnection(const std::string&) override
    {
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


TEST_CASE("input-test-error", "[input]")
{
    SECTION("empty is ok")
    {
        auto config = config::InputSystem
        {
            {},
            {
                {
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(loaded);
    }

    SECTION("only actions is ok")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::WithinZeroToOne}
            },
            {
                {
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(loaded);
    }

    // todo(Gustav): should we ignore empty units? leave them empty...
    SECTION("empty unit is ok")
    {
        auto config = config::InputSystem
        {
            {
            },
            {
                {
                    "empty",
                    {
                    },
                    {
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(loaded);
    }

    SECTION("reference missing action")
    {
        auto config = config::InputSystem
        {
            {
            },
            {
                {
                    "keyboard",
                    {
                        config::KeyboardDef{}
                    },
                    {
                        config::KeyBindDef{"no_action", 0, Key::X}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Unknown action no_action (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }

    SECTION("reference invalid unit")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::WithinZeroToOne}
            },
            {
                {
                    "keyboard",
                    {
                        config::KeyboardDef{}
                    },
                    {
                        config::KeyBindDef{"hello", 1, Key::X}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Invalid unit 1 (action: hello) (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }

    SECTION("detect invalid key")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::WithinZeroToOne}
            },
            {
                {
                    "keyboard",
                    {
                        config::KeyboardDef{}
                    },
                    {
                        config::KeyBindDef{"hello", 0, -2}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Invalid bind to unbound: -2 (action: hello) (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }

    SECTION("detect invalid mouse button")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::WithinZeroToOne}
            },
            {
                {
                    "keyboard",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::KeyBindDef{"hello", 0, 123}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Invalid mouse button: 123 (action: hello) (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }

    SECTION("detect invalid axis for keyboard")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::Infinite}
            },
            {
                {
                    "keyboard",
                    {
                        config::KeyboardDef{}
                    },
                    {
                        config::AxisBindDef{"hello", 0, AxisType::Ball, 0, 0}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Keyboard doesn't support axis binds (action: hello) (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }

    SECTION("detect invalid axis for mouse")
    {
        auto config = config::InputSystem
        {
            {
                {"hello", "hello", Range::Infinite}
            },
            {
                {
                    "keyboard",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::AxisBindDef{"hello", 0, AxisType::Ball, 0, 0}
                    }
                }
            }
        };

        auto loaded = Load(config);
        REQUIRE(StringEq(loaded.error(), "Invalid type for mouse (action: hello) (mapping: keyboard)"));
        CHECK_FALSE(loaded);
    }
}


TEST_CASE("input-test-simple", "[input]")
{
    auto test_platform = TestPlatform{};

    SECTION("test inverted axis")
    {
        auto config = config::InputSystem
        {
            {
                {"ax", "ax", Range::Infinite}
            },
            {
                {
                    "mouse",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::AxisBindDef{"ax", 0, Axis::X, 1.0f, true}
                    }
                }
            }
        };

        auto loaded = Load(config);
        INFO(loaded.error());
        REQUIRE(loaded);

        auto& sys = *loaded.value;
        auto player = sys.AddPlayer();
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

        const auto input = GENERATE(1.0f, -1.0f);
        INFO("input: " << input);

        sys.OnMouseAxis(Axis::X, input, 400 + input);
        CHECK(MapEq(GetTable(&sys, player), {
            {"ax", -input}
        }));
    }

    SECTION("test non-inverted axis")
    {
        auto config = config::InputSystem
        {
            {
                {"ax", "ax", Range::Infinite}
            },
            {
                {
                    "mouse",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::AxisBindDef{"ax", 0, Axis::X, 1.0f, false}
                    }
                }
            }
        };

        auto loaded = Load(config);
        INFO(loaded.error());
        REQUIRE(loaded);

        auto& sys = *loaded.value;
        auto player = sys.AddPlayer();
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

        const auto input = GENERATE(1.0f, -1.0f);
        INFO("input: " << input);

        sys.OnMouseAxis(Axis::X, input, 400 + input);
        CHECK(MapEq(GetTable(&sys, player), {
            {"ax", input}
        }));
    }

    SECTION("mouse relative respects sensitivity")
    {
        const auto sens = GENERATE(0.1f, 1.0f, 2.0f);
        INFO("sens: " << sens);
        auto config = config::InputSystem
        {
            {
                {"ax", "ax", Range::Infinite}
            },
            {
                {
                    "mouse",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::AxisBindDef{"ax", 0, Axis::X, sens, false}
                    }
                }
            }
        };

        auto loaded = Load(config);
        INFO(loaded.error());
        REQUIRE(loaded);

        auto& sys = *loaded.value;
        auto player = sys.AddPlayer();
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

        sys.OnMouseAxis(Axis::X, 1.0f, 0.0f);
        CHECK(MapEq(GetTable(&sys, player), {
            {"ax", sens}
        }));
    }

    SECTION("ignore sensitivity axis for 'mouse position'")
    {
        const auto sens = GENERATE(0.1f, 1.0f, 2.0f);
        INFO("sens: " << sens);
        auto config = config::InputSystem
        {
            {
                {"ax", "ax", Range::WithinNegativeOneToPositiveOne}
            },
            {
                {
                    "mouse",
                    {
                        config::MouseDef{}
                    },
                    {
                        config::AxisBindDef{"ax", 0, Axis::X, sens, false}
                    }
                }
            }
        };

        auto loaded = Load(config);
        INFO(loaded.error());
        REQUIRE(loaded);

        auto& sys = *loaded.value;
        auto player = sys.AddPlayer();
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

        sys.OnMouseAxis(Axis::X, 0.5f, 1.0f);
        CHECK(MapEq(GetTable(&sys, player), {
            {"ax", 1.0f}
        }));
    }
}


TEST_CASE("input-test-big", "[input]")
{
    constexpr auto JOYSTICK_HANDLE = static_cast<JoystickId>(42);
    constexpr int JOYSTICK_START = 0;
    constexpr int JOYSTICK_SHOOT = 1;
    const std::string JOYSTICK_GUID = "joystick-guid";
    constexpr int JOYSTICK_MOVE = 0;
    constexpr int JOYSTICK_LOOK = 1;

    auto config = config::InputSystem
    {
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
                    config::JoystickDef{JOYSTICK_START, JOYSTICK_GUID},
                    config::KeyboardDef{} // keyboard that does nothing
                },
                {
                    config::KeyBindDef{"shoot", 0, JOYSTICK_SHOOT},
                    config::AxisBindDef{"move", 0, AxisType::GeneralAxis, 0, JOYSTICK_MOVE},
                    config::AxisBindDef{"look", 0, AxisType::GeneralAxis, 0, JOYSTICK_LOOK}
                }
            }
        }
    };

    auto loaded = Load(config);
    REQUIRE(loaded);
    auto& sys = *loaded.value;

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

