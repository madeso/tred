/** @file
Classes for input handling.
 */

#pragma once
#include <string>


namespace input
{
struct InputAction;
struct GlobalToggle;

enum class BindType
{
    RANGE,
    AXIS
};

std::string ToString(const BindType& bt);

std::ostream& operator<<(std::ostream& ss, const BindType& bt);

/** A bind.
 */
struct Bind
{
    Bind(InputAction* action, BindType type);
    ~Bind();

    float value() const;
    void set_value(float v);

    BindType type() const;
    InputAction* action();

    InputAction* action_;
    BindType type_;
    GlobalToggle* toggle_;
    float value_;
};

}  // namespace input
