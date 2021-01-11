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


Table GetTable(InputSystem* system, PlayerHandle player)
{
    Table table;
    system->UpdateTable(player, &table);
    return table;
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
            // actions
            // todo(Gustav): add multiple key bindings
            // todo(Gustav): add groupings... (game: use this group now) car/walk/swim ...
            // todo(Gustav): add categories/tags (better name) (input: use theese functions now): point selection, grid selection...
            // todo(Gustav): support virtual keys/axis for touch controls like 'sinput'
            {
                {"shoot", "var_shoot", Range::WithinZeroToOne},
                {"look", "var_look", Range::WithinNegativeOneToPositiveOne},
                {"move", "var_move", Range::WithinNegativeOneToPositiveOne},
            },

            // controller setup (bind)
            {
                {
                    "mouse+keyboard",
                    // two button converters
                    {
                        "move"
                    },
                    // keyboards
                    {
                        config::KeyboardDef
                        {
                            Key::RETURN,
                            {
                                {"shoot", Key::A},
                                {"move-", Key::LEFT},
                                {"move+", Key::RIGHT},
                            }
                        }
                    },
                    // mouses
                    {
                        config::MouseDef
                        {
                            MouseButton::LEFT,
                            {
                                {"look", Axis::X}
                            },
                            // no mouse wheeels
                            {},
                            // no mouse buttons
                            {}
                        }
                    },
                    // joysticks
                    {}
                },
                {
                    "joystick",
                    // two button converters
                    {},
                    // keyboards
                    {},
                    // mouses
                    {},
                    // joysticks
                    {
                        config::JoystickDef
                        {
                            JOYSTICK_START, JOYSTICK_GUID,
                            // axis
                            {
                                {"move", JOYSTICK_MOVE},
                                {"look", JOYSTICK_LOOK},
                            },
                            // buttons
                            {
                                {"shoot", JOYSTICK_SHOOT}
                            },
                            // no hats
                            {},
                            // no balls
                            {}
                        }
                    }
                }
            }
        }
    };

    auto test_platform = TestPlatform{};

    auto player = sys.AddPlayer();

    SECTION("no assigned control is valid")
    {
        const auto table = GetTable(&sys, player);

        REQUIRE(MapEq(table.data, {
        }));
    }

    SECTION("test auto assignments")
    {
        REQUIRE_FALSE(sys.IsConnected(player));

        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);
        
        REQUIRE(sys.IsConnected(player));

        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
        
        test_platform.joysticks[JOYSTICK_HANDLE] = JOYSTICK_GUID;
        REQUIRE(test_platform.joysticks.size() == 1);

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);
        sys.UpdatePlayerConnections(UnitDiscovery::FindHighest, &test_platform);

        // joystick was grabbed
        REQUIRE(test_platform.joysticks.size() == 0);

        // player is still connected
        REQUIRE(sys.IsConnected(player));


        // keyboard input is ignored...

        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // but joystick works...

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("test explicit assignments keyboard")
    {
        const auto use_keyboard = GENERATE(true, false);
        INFO("Use keyboard " << use_keyboard);
        auto press = [&](bool down)
        {
            if(use_keyboard) { sys.OnKeyboardKey(Key::RETURN, down); }
            else { sys.OnMouseButton(MouseButton::LEFT, down); }
        };

        REQUIRE_FALSE(sys.IsConnected(player));

        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);
        REQUIRE_FALSE(sys.IsConnected(player));

        press(true);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE_FALSE(sys.IsConnected(player));

        press(false);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE(sys.IsConnected(player));

        // keyboard input works
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
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
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);
        REQUIRE(test_platform.joysticks.size() == 1);

        // and joystick is ignored
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
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

        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);
        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, true);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_START, false);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE(sys.IsConnected(player));
        REQUIRE(test_platform.joysticks.size() == 0);

        // keyboard input works
        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnJoystickButton(JOYSTICK_HANDLE, JOYSTICK_SHOOT, false);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // press start
        press(true);
        press(false);

        // update but it shouldn't be picked up
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        // and keyboard is ignored
        sys.OnKeyboardKey(Key::A, true);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }


    SECTION("no input")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        const auto table = GetTable(&sys, player);

        REQUIRE(MapEq(table.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("push button")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        sys.OnKeyboardKey(Key::A, true);

        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        sys.OnKeyboardKey(Key::A, false);

        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("move mouse")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        sys.OnMouseAxis(Axis::X, 2.0f);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 1.0f},
            {"var_move", 0.0f}
        }));

        sys.OnMouseAxis(Axis::X, 0.0f);
        REQUIRE(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("move with keyboard")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        // start clean
        CHECK(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // left down
        sys.OnKeyboardKey(Key::LEFT, true);
        CHECK(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", -1.0f}
        }));

        // left and right down
        sys.OnKeyboardKey(Key::RIGHT, true);
        CHECK(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));

        // only right down
        sys.OnKeyboardKey(Key::LEFT, false);
        CHECK(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 1.0f}
        }));

        // nothing down
        sys.OnKeyboardKey(Key::RIGHT, false);
        CHECK(MapEq(GetTable(&sys, player).data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }
}

