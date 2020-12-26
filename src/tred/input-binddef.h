/** @file
Classes for input handling.
 */

#pragma once
#include <string>
#include <cassert>

#include "tred/input-config.h"



namespace input {

/** Definition of a bind.
 */

template <typename Type>
struct BindDef {
  template<typename D>
  BindDef(const std::string& id, const Type type, const D& d)
      : id_(id),
        type_(type),
        invert_(d.invert),
        scale_(d.scale) {
      }
  const std::string& id() const {
        return id_;
  }
  const Type type() const {
        return type_;
  }

  bool invert() const {
        return invert_;
  }

  float scale() const {
        return scale_;
  }

   std::string id_;
  Type type_;
  bool invert_;
  float scale_;
};

}  // namespace input




