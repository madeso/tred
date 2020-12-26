#include "tred/input-connectedunits.h"

#include <cassert>
#include <string>

#include "tred/input-activeunit.h"
#include "tred/input-activelist.h"





namespace input {

ConnectedUnits::ConnectedUnits(std::shared_ptr<ActiveList> actives)
    : actives_(actives) {
    assert(actives);
}

void ConnectedUnits::Add(std::shared_ptr<ActiveUnit> unit) {
    assert(unit);
  units_.push_back(unit);
}

void ConnectedUnits::UpdateTable(Table* table) {
    assert(table);
  assert(actives_);

  // not really relevant but this is great for error checking
  if (units_.empty()) {
    const std::string error = "No units connected for table update to be completed";
    throw error;
  }

  actives_->UpdateTable(table);
}

void ConnectedUnits::Update(float dt) {
    assert(actives_);

  actives_->Update(dt);
}

bool ConnectedUnits::IsEmpty() const {
    return units_.empty();
}

}  // namespace input

