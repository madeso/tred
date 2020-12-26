/** @file
Classes for input handling.
 */

#pragma once
#include "tred/input-activeunit.h"


namespace input
{
/** A Dummy active unit.
 */
struct DummyActiveUnit : public ActiveUnit
{
    /** Rumble.
   */
    void Rumble() override;
};

}  // namespace input
