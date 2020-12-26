/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/input-key.h"
#include "tred/input-axis.h"
#include "tred/input-keyconfigs.h"
#include "tred/input-actionmap.h"


struct Table;

namespace input {

namespace config{ struct InputSystem; }

struct Player;
struct GlobalToggle;
struct InputDirector;

/** the master struct that controls the input system.
 */
struct InputSystem {
   /** Constructor.
   */
  explicit InputSystem(const config::InputSystem& config);

  /** Destructor.
   */
  ~InputSystem();

  /** Get a action.
  @param name the name of the action
  @returns the action
   */
  std::shared_ptr<GlobalToggle> GetAction(const std::string& name);

  /** Setups a input unit for a player.
  @param playerName the name of the player
  @param inputName the name of the input
   */
  void SetUnitForPlayer(const std::string& playerName,
                        const std::string& inputName);

  void Update(float dt);

  /** Send a keyboard event.
  @param key the key to act upon
  @param down true if it is down, false if not
   */
  void OnKeyboardKey(Key key, bool down);

  /** Send a mouse axis.
  @param axis the axis
  @param value the value
   */
  void OnMouseAxis(Axis axis, float value);

  /** Send a mouse button.
  @param button the button to act upon
  @param down true if it is down, false if not
   */
  void OnMouseButton(MouseButton button, bool down);

  /** Send a joystick pov.
  @param type the pov type
  @param hat the joystick pov hat
  @param joystick the joystick id
  @param value the value
   */
  void OnJoystickPov(Axis type, int hat, int joystick, float value);

  /** Send a joystick button.
  @param button the button to act upon
  @param joystick the joystick id
  @param down true if it is down, false if not
   */
  void OnJoystickButton(int button, int joystick, bool down);

  /** Send a joystick axis.
  @param axis the axis
  @param joystick the joystick id
  @param value the value
   */
  void OnJoystickAxis(int axis, int joystick, float value);

  /** Get a player.
  @param name the name of the player
  @returns the player
   */
  std::shared_ptr<Player> GetPlayer(const std::string& name);

  /** Add a player.
  @name the name of the player.
   */
  void AddPlayer(const std::string& name);

   InputActionMap actions_;
  std::map<std::string, std::shared_ptr<Player>> players_;
  KeyConfigs configs_;
  std::unique_ptr<InputDirector> input_;
};

}  // namespace input




