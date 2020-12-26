/** @file
Classes for input handling.
 */

#pragma once
#include <memory>
#include <cassert>

#include "tred/input-bind.h"



namespace input {
template <typename T>
struct TRangeBind {
   TRangeBind(T button, std::shared_ptr<Bind> bind, bool invert, float scale)
      : button_(button), bind_(bind), invert_(invert), scale_(scale) {
        assert(bind);
  }

  const T GetType() const {
        return button_;
  }

  std::shared_ptr<Bind> bind() {
        assert(bind_);
    return bind_;
  }

  bool invert() const {
        return invert_;
  }

  float scale() const {
        return scale_;
  }

   T button_;
  std::shared_ptr<Bind> bind_;

  bool invert_;
  float scale_;
};

}  // namespace input




