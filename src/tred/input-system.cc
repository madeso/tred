#include "tred/input-system.h"

#include <cassert>
#include <fstream>  // NOLINT for loading data
#include "fmt/format.h"

#include "tred/input-config.h"

#include "tred/input-director.h"
#include "tred/input-player.h"
#include "tred/input-keyconfig.h"



namespace input {

void Load(InputSystem* sys, const config::InputSystem& root) {
  assert(sys);
  
  for (const auto& name : root.players) {
    sys->AddPlayer(name);
  }
}

InputSystem::InputSystem(const config::InputSystem& config) : input_(new InputDirector()) {
    Load(&actions_, config.actions);
  Load(&configs_, config.keys, actions_);
  Load(this, config);
}

InputSystem::~InputSystem() {
    players_.clear();
}

std::shared_ptr<GlobalToggle> InputSystem::GetAction(const std::string& name) {
    return actions_.GetGlobalToggle(name);
}

void InputSystem::SetUnitForPlayer(const std::string& playerName,
                                   const std::string& inputname) {
    auto res = players_.find(playerName);
  if (res == players_.end()) {
    const std::string error = fmt::format("Unable to find player {}", playerName);
    throw error;
  }
  std::shared_ptr<Player> player = res->second;

  auto config = configs_.Get(inputname);
  player->set_units(config->Connect(actions_, input_.get()));
}

void InputSystem::Update(float dt) {
    actions_.Update();

  for (auto p : players_) {
    p.second->Update(dt);
  }
}

void InputSystem::OnKeyboardKey(Key key, bool down) {
    input_->OnKeyboardKey(key, down);
}

void InputSystem::OnMouseAxis(Axis axis, float value) {
    input_->OnMouseAxis(axis, value);
}

void InputSystem::OnMouseButton(MouseButton button, bool down) {
    input_->OnMouseButton(button, down);
}

void InputSystem::OnJoystickPov(Axis type, int hat, int joystick, float value) {
    input_->OnJoystickPov(type, hat, joystick, value);
}

void InputSystem::OnJoystickButton(int button, int joystick, bool down) {
    input_->OnJoystickButton(button, joystick, down);
}

void InputSystem::OnJoystickAxis(int axis, int joystick, float value) {
    input_->OnJoystickAxis(axis, joystick, value);
}

std::shared_ptr<Player> InputSystem::GetPlayer(const std::string& name) {
    auto res = players_.find(name);
  if (res == players_.end()) {
    const std::string error = fmt::format("Unable to find player {}", name);
    throw error;
  }
  assert(res->second);
  return res->second;
}

void InputSystem::AddPlayer(const std::string& name) {
    std::shared_ptr<Player> p(new Player());
  players_.insert(std::make_pair(name, p));
}

}  // namespace input

