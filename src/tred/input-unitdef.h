// Euphoria - Copyright (c) Gustav

/** @file

 */

#ifndef EUPHORIA_INPUT_UNITDEF_H_
#define EUPHORIA_INPUT_UNITDEF_H_

#include <memory>



namespace input {

struct InputDirector;
struct ActiveUnit;
struct BindMap;

/** Definition of a certain input unit.
Usually loaded from a file.
 */
struct UnitDef {
 public:
  /** Destructor.
   */
  virtual ~UnitDef();

  /** Create a active unit.
  @param director the input director
  @returns the active unit.
   */
  virtual std::shared_ptr<ActiveUnit> Create(InputDirector* director,
                                             BindMap* map) = 0;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_UNITDEF_H_
