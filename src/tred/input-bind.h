// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_BIND_H_
#define EUPHORIA_INPUT_BIND_H_

#include <string>



namespace input {

struct InputAction;
struct GlobalToggle;

enum class BindType { RANGE, AXIS };

std::string ToString(const BindType& bt);

std::ostream& operator<<(std::ostream& ss, const BindType& bt);

/** A bind.
 */
struct Bind {
 public:
  Bind(InputAction* action, BindType type);
  ~Bind();

  float value() const;
  void set_value(float v);

  BindType type() const;
  InputAction* action();

 private:
  InputAction* action_;
  BindType type_;
  GlobalToggle* toggle_;
  float value_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_BIND_H_
