// Euphoria - Copyright (c) Gustav

#include "tred/input-activelist.h"
#include <cassert>

#include "tred/input-activeaxis.h"
#include "tred/input-activerange.h"
#include "tred/input-activerangetoaxis.h"
#include "tred/input-activeaxistorange.h"
#include "tred/input-activemasteraxis.h"
#include "tred/input-activemasterrange.h"

#include "tred/input-action.h"
#include "tred/input-table.h"



namespace input {

void ActiveList::Add(std::shared_ptr<ActiveRange> range) {
  assert(this);
  range_binds_.push_back(range);
}

void ActiveList::Add(std::shared_ptr<ActiveAxis> axis) {
  assert(this);
  axis_binds_.push_back(axis);
}

void ActiveList::Add(std::shared_ptr<ActiveAxisToRange> axis) {
  assert(this);
  axis_to_range_binds_.push_back(axis);
}

void ActiveList::Add(std::shared_ptr<ActiveRangeToAxis> axis) {
  assert(this);
  range_to_axis_binds_.push_back(axis);
}

void ActiveList::Add(std::shared_ptr<ActiveMasterAxis> axis) {
  assert(this);
  master_axis_binds_.push_back(axis);
}

void ActiveList::Add(std::shared_ptr<ActiveMasterRange> axis) {
  assert(this);
  master_range_binds_.push_back(axis);
}

void ActiveList::UpdateTable(Table* table) {
  assert(this);
  assert(table);

  for (auto b : master_range_binds_) {
    table->Set(b->action().scriptvarname(), b->state());
  }

  for (auto b : master_axis_binds_) {
    table->Set(b->action().scriptvarname(), b->state());
  }
}

void ActiveList::Update(float dt) {
  for (auto range : range_binds_) {
    range->Update(dt);
  }
  for (auto axis : axis_binds_) {
    axis->Update(dt);
  }
  for (auto axis : axis_to_range_binds_) {
    axis->Update(dt);
  }
  for (auto axis : range_to_axis_binds_) {
    axis->Update(dt);
  }
  for (auto axis : master_axis_binds_) {
    axis->Update(dt);
  }
  for (auto axis : master_range_binds_) {
    axis->Update(dt);
  }
}

}  // namespace input

