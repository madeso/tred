/** @file
Classes for input handling.
 */

#pragma once
#include <memory>
#include <cassert>

#include "tred/input-bind.h"



namespace input {

template <typename T>
struct TAxisBind {
   TAxisBind(T axis, std::shared_ptr<Bind> bind, bool invert, float scale)
      : axis_(axis), bind_(bind), invert_(invert), scale_(scale) {
        assert(bind);
  }

  const T GetType() const {
        return axis_;
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

   T axis_;
  std::shared_ptr<Bind> bind_;
  bool invert_;
  float scale_;
};

}  // namespace input




