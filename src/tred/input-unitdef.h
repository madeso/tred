/** @file

 */

#pragma once
#include <memory>


namespace input
{
struct InputDirector;
struct ActiveUnit;
struct BindMap;

/** Definition of a certain input unit.
Usually loaded from a file.
 */
struct UnitDef
{
    /** Destructor.
   */
    virtual ~UnitDef();

    /** Create a active unit.
  @param director the input director
  @returns the active unit.
   */
    virtual std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map) = 0;
};

}  // namespace input
