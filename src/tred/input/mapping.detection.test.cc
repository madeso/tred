#include "tred/input/mapping.detection.h"

#include "catch.hpp"

using namespace input;


namespace
{

enum class Type
{
    Joystick, KeyboardOrMouse
};


struct Def
{
    Type type;
    bool can_detect;

    Def(Type typ, bool det) : type(typ), can_detect(det) {}
};

bool MappingDetectionTest(const std::vector<Def>& definitions)
{
    return MappingDetection
    (
        definitions,
        [](const Def& d) -> bool { return d.type == Type::Joystick; },
        [](const Def& d) -> bool { return d.can_detect; }
    );
}

}


TEST_CASE("input-mapping-detection-test", "[input.mapping]")
{
    // no defs are not detected
    CHECK_FALSE(MappingDetectionTest({}));

    // gamepad
    SECTION("one gamepad")
    {
        CHECK_FALSE(MappingDetectionTest({
            {Type::Joystick, false}
        }));
        CHECK(MappingDetectionTest({
            {Type::Joystick, true}
        }));
    }

    SECTION("one keyboard")
    {
        CHECK_FALSE(MappingDetectionTest({
            {Type::KeyboardOrMouse, false}
        }));
        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, true}
        }));
    }

    SECTION("keyboard + mouse")
    {
        CHECK_FALSE(MappingDetectionTest({
            {Type::KeyboardOrMouse, false},
            {Type::KeyboardOrMouse, false}
        }));

        // either keyboard or mouse works to detect the mapping
        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, false},
            {Type::KeyboardOrMouse, true}
        }));
        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, true},
            {Type::KeyboardOrMouse, false}
        }));

        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, true},
            {Type::KeyboardOrMouse, true}
        }));
    }

    SECTION("two gamepads on same player")
    {
        CHECK_FALSE(MappingDetectionTest({
            {Type::Joystick, false},
            {Type::Joystick, false}
        }));
        CHECK_FALSE(MappingDetectionTest({
            {Type::Joystick, false},
            {Type::Joystick, true}
        }));
        CHECK_FALSE(MappingDetectionTest({
            {Type::Joystick, true},
            {Type::Joystick, false}
        }));

        // both gamepads needs detection for gamepad to work
        CHECK(MappingDetectionTest({
            {Type::Joystick, true},
            {Type::Joystick, true}
        }));
    }

    SECTION("gamepad + keyboard")
    {
        CHECK_FALSE(MappingDetectionTest({
            {Type::KeyboardOrMouse, false},
            {Type::Joystick, false}
        }));

        // only keyboard = not detected (as no id could be assigned)
        // only gamepad = detected

        CHECK_FALSE(MappingDetectionTest({
            {Type::KeyboardOrMouse, true},
            {Type::Joystick, false}
        }));
        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, false},
            {Type::Joystick, true}
        }));

        // both is also ok
        CHECK(MappingDetectionTest({
            {Type::KeyboardOrMouse, true},
            {Type::Joystick, true}
        }));
    }
}

