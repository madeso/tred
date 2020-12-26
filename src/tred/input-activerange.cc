#include "tred/input-activerange.h"
#include <cassert>
#include <string>
#include "fmt/format.h"
#include "tred/input-bind.h"

#include "tred/input-action.h"



namespace input {

ActiveRange::ActiveRange(InputAction* action, Bind* range)
    : InputActive(action), range_(range) {
    assert(range_);

  if (range_->type() != BindType::RANGE) {
    const std::string error = fmt::format("bound type for {} is not a range, is {}", action->name(), range_->type());
    throw error;
  }
}

void ActiveRange::Update(float) {
    assert(range_);
  set_state(range_->value());
}

}  // namespace input

