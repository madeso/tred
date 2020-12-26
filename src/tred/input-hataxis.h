/** @file
Classes for input definitions.
 */

#pragma once
#include "tred/input-axis.h"



namespace input {

struct HatAxis {
   HatAxis(int hat, Axis axis);

  Axis axis() const;
  int hat() const;

  bool operator<(const HatAxis& rhs) const;

   int hat_;
  Axis axis_;
};

}  // namespace input




