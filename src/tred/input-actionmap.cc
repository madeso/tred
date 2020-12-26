// Euphoria - Copyright (c) Gustav

#include "tred/input-actionmap.h"
#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-range.h"
#include "tred/input-action.h"
#include "tred/input-globaltoggle.h"



namespace input {

InputActionMap::InputActionMap() { assert(this); }

void InputActionMap::Update() {
  assert(this);
  for (auto t : toggles_) {
    (t.second)->Update();
  }
}

void InputActionMap::Add(const std::string& name,
                         std::shared_ptr<InputAction> action) {
  assert(this);
  assert(action);
  actions_.insert(std::make_pair(name, action));
  if (action->global()) {
    std::shared_ptr<GlobalToggle> toggle(new GlobalToggle(action));
    toggles_.insert(std::make_pair(name, toggle));
  }
}

std::shared_ptr<InputAction> InputActionMap::Get(
    const std::string& name) const {
  assert(this);
  auto res = actions_.find(name);
  if (res == actions_.end()) {
    const std::string error = fmt::format("Unable to find action: {}", name);
    throw error;
  }

  assert(res->second);
  return res->second;
}

std::shared_ptr<GlobalToggle> InputActionMap::GetGlobalToggle(
    const std::string& name) const {
  assert(this);
  auto res = toggles_.find(name);
  if (res == toggles_.end()) {
    const std::string error = fmt::format("Unable to find toggle: {}", name);
    throw error;
  }

  assert(res->second);
  return res->second;
}

std::vector<std::shared_ptr<InputAction>> InputActionMap::GetActionList()
    const {
  assert(this);
  std::vector<std::shared_ptr<InputAction>> ret;
  for (auto a : actions_) {
    ret.push_back(a.second);
  }
  return ret;
}

//////////////////////////////////////////////////////////////////////////

void Load(InputActionMap* map, const input::config::ActionMap& root) {
  assert(map);
  
  for (const auto& d : root.actions)
  {
    const std::string name = d.name;
    const std::string varname = d.var;
    const bool global = d.global;
    const Range range = d.range;
    std::shared_ptr<InputAction> action(
        new InputAction(name, varname, range, global));
    map->Add(name, action);
  }
}

}  // namespace input

