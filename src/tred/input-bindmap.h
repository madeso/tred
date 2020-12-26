// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_BINDMAP_H_
#define EUPHORIA_INPUT_BINDMAP_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "tred/input-actionmap.h"

#include "tred/input-bind.h"

#include "tred/input-activeaxis.h"
#include "tred/input-activerange.h"



struct Table;

namespace input {

struct InputAction;
struct ActiveList;

/** A container for all the actives in the game.
 */
struct BindMap {
 public:
  BindMap(const InputActionMap& actions, ActiveList* actives);
  std::shared_ptr<Bind> GetBindByName(const std::string& name);

 private:
  void AddRange(std::shared_ptr<InputAction> action, ActiveList* actives);
  void AddAxis(std::shared_ptr<InputAction> action, ActiveList* actives);

  std::map<std::string, std::shared_ptr<Bind>> binds_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_BINDMAP_H_
