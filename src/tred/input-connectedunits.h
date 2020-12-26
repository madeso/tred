/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <memory>


struct Table;

namespace input
{
struct ActiveUnit;
struct ActiveList;

/** Contains a list of active units.
 */
struct ConnectedUnits
{
    /** Constructor.
   */
    explicit ConnectedUnits(std::shared_ptr<ActiveList> actives);

    /** Updates all connected units.
  @param table the table to update
   */
    void UpdateTable(Table* table);

    void Update(float dt);

    /** Add a unit.
  @param unit the unit to add
   */
    void Add(std::shared_ptr<ActiveUnit> unit);

    /** Are there any connected units?
  @returns true if there are no active units, false if not
   */
    bool IsEmpty() const;

    std::vector<std::shared_ptr<ActiveUnit>> units_;
    std::shared_ptr<ActiveList> actives_;
};

}  // namespace input
