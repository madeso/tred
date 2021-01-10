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
    void RemoveJustPressed() override
    {
    }

    // todo(Gustav): rename to GetAvailableJoysticks
    std::vector<JoystickId> ActiveAndFreeJoysticks() override
    {
        return {};
    }

    bool MatchUnit(JoystickId, const std::string&) override
    {
        return false;
    }

    bool WasJustPressed(JoystickId, int) override
    {
        return false;
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
    auto sys = InputSystem
    {
        {
            // actions
            // todo(Gustav): add back support for globals, input not tied to player
            // todo(Gustav): add groupings... car/walk/swim ...?
            // todo(Gustav): how does "converters" work? extend to 2d
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
                    {
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
    }

    SECTION("test explicit assignments")
    {
        REQUIRE_FALSE(sys.IsConnected(player));

        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);
        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnKeyboardKey(Key::RETURN, true);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE_FALSE(sys.IsConnected(player));

        sys.OnKeyboardKey(Key::RETURN, false);
        sys.UpdatePlayerConnections(UnitDiscovery::PressToActivate, &test_platform);

        REQUIRE(sys.IsConnected(player));
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

        const auto before = GetTable(&sys, player);

        sys.OnKeyboardKey(Key::A, false);

        const auto after = GetTable(&sys, player);

        REQUIRE(MapEq(before.data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
        REQUIRE(MapEq(after.data, {
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
        const auto before = GetTable(&sys, player);

        sys.OnMouseAxis(Axis::X, 0.0f);
        const auto after = GetTable(&sys, player);

        REQUIRE(MapEq(before.data, {
            {"var_shoot", 0.0f},
            {"var_look", 1.0f},
            {"var_move", 0.0f}
        }));
        REQUIRE(MapEq(after.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }

    SECTION("move with keyboard")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        const auto start = GetTable(&sys, player);

        sys.OnKeyboardKey(Key::LEFT, true);
        const auto left = GetTable(&sys, player);

        sys.OnKeyboardKey(Key::RIGHT, true);
        const auto both = GetTable(&sys, player);

        sys.OnKeyboardKey(Key::LEFT, false);
        const auto right = GetTable(&sys, player);

        sys.OnKeyboardKey(Key::RIGHT, false);
        const auto after = GetTable(&sys, player);

        CHECK(MapEq(start.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
        CHECK(MapEq(left.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", -1.0f}
        }));
        CHECK(MapEq(both.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
        CHECK(MapEq(right.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 1.0f}
        }));
        CHECK(MapEq(after.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f},
            {"var_move", 0.0f}
        }));
    }
}

