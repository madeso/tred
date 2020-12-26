#include "tred/handle.h"

#include <sstream>
#include <functional>
#include "fmt/format.h"

#include "catch.hpp"

#include "tred/input-system.h"
#include "tred/input-player.h"
#include "tred/input-config.h"
#include "tred/input-table.h"

#include "catchy/falsestring.h"

using namespace input;
using namespace Catch::Matchers;
using namespace catchy;

Table GetTable(std::shared_ptr<Player> player)
{
    Table table;
    player->UpdateTable(&table);
    return table;
}

template<typename K, typename V, typename C>
FalseString BaseMapEquals(const std::map<K, V>& lhs, const std::map<K, V>& rhs, const C& compare)
{
    std::vector<std::string> issues;

    for(const auto& lhsp: lhs)
    {
        auto found = rhs.find(lhsp.first);
        if(found == rhs.end())
        {
            issues.emplace_back(fmt::format("{} missing in rhs", lhsp.first));
        }
        else
        {
            const catchy::FalseString result = compare(lhsp.second, found->second);
            if(result == false)
            {
                issues.emplace_back(fmt::format("{} was different: {}", lhsp.first, result.reason));
            }
        }
    }

    for(const auto& rhsp: rhs)
    {
        auto found = lhs.find(rhsp.first);
        if(found == lhs.end())
        {
            issues.emplace_back(fmt::format("{} missing in lhs", rhsp.first));
        }
    }

    if(issues.empty()) { return FalseString::True();}

    std::ostringstream ss;
    bool first = true;
    for(const auto& s: issues)
    {
        if(first) first = false;
        else ss << "\n";
        ss << s;
    }
    
    return catchy::FalseString::False(ss.str());
};


catchy::FalseString MapEquals(const std::map<std::string, float>& lhs, const std::map<std::string, float>& rhs)
{
    return BaseMapEquals(lhs, rhs, [](float l, float r) -> FalseString
    {
        const auto eps = std::numeric_limits<float>::epsilon() * 100;
        const bool same = std::abs(r-l) < eps;
        if(same) return FalseString::True();
        return FalseString::False(fmt::format("{} != {}", l, r));
    });
}


TEST_CASE("input-test", "[input]")
{
    auto sys = InputSystem
    {
        {
            // actions
            // todo(Gustav): split global and player input
            // todo(Gustav): add groupings... car/walk/swim ...?
            {
                {"shoot", "var_shoot", Range::WithinZeroToOne, false},
                {"look", "var_look", Range::WithinNegativeOneToPositiveOne, false}
            },

            // controller setup (bind)
            {
                {
                    "mouse+keyboard",
                    {
                        config::KeyboardDef
                        {
                            {
                                {{"shoot", "action_name"}, Key::A}
                            }
                        },
                        config::MouseDef
                        {
                            {
                                {{"look", "mouse_name"}, Axis::X}
                            },
                            // no mouse buttons
                            {}
                        }
                    }
                }
            },

            // players
            // todo(Gustav): remove list of player... concept is good, list is bad
            {
                "player"
            }
        }
    };

    auto player = sys.GetPlayer("player");

    SECTION("no assigned control is valid")
    {
        const auto table = GetTable(player);

        REQUIRE(MapEquals(table.data, {
        }));
    }

    SECTION("no input")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        const auto table = GetTable(player);

        REQUIRE(MapEquals(table.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f}
        }));
    }

    SECTION("push button")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        sys.OnKeyboardKey(Key::A, true);
        sys.Update(0.1f);

        const auto before = GetTable(player);

        sys.OnKeyboardKey(Key::A, false);
        sys.Update(0.1f);

        const auto after = GetTable(player);

        REQUIRE(MapEquals(before.data, {
            {"var_shoot", 1.0f},
            {"var_look", 0.0f}
        }));
        REQUIRE(MapEquals(after.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f}
        }));
    }

    SECTION("move mouse")
    {
        // press enter on playscreen
        sys.SetUnitForPlayer(player, "mouse+keyboard");

        sys.OnMouseAxis(Axis::X, 2.0f);
        sys.Update(0.1f);

        const auto before = GetTable(player);

        sys.OnMouseAxis(Axis::X, 0.0f);
        sys.Update(0.1f);

        const auto after = GetTable(player);

        REQUIRE(MapEquals(before.data, {
            {"var_shoot", 0.0f},
            {"var_look", 1.0f}
        }));
        REQUIRE(MapEquals(after.data, {
            {"var_shoot", 0.0f},
            {"var_look", 0.0f}
        }));
    }
}

