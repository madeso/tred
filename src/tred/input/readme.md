# Input System

## issues/good things

* hard to set up bindable ui/complex setup -> simplify binding to archetypes?
* how do different control types work? on foot, car, aeroplane, player needs to send type
* need ability to toggle mouse capture: esc/~/tab

* once it's running the api works great


## types

* A digital button. Either true or false
* A analog button. Can detect how hard it is pressed?
* Trigger.

* relative input. use moves input to acchieve delta
* absolute input. use stops moving to stop delta.

## converters

* range01 + range01 = range11
* range11 + range11 = range2
* keep a 2d range within a certain value, either a rect or a circle
* deadzone converter
* change value over time

## units

### range01

A digital button, a analog button or a trigger

### range2

### point2

## devices

### keyboard

<img src="../../../data/icons/keyboard.svg" alt="touch screen" width="50"/>

* button **range01**

### mouse

<img src="../../../data/icons/mouse.svg" alt="touch screen" width="50"/>

* cursor
  * relative **range2**
  * absolute **point2**
* button **range01**

### joystick

<img src="../../../data/icons/joystick.svg" alt="touch screen" width="50"/>

### gamepad

<img src="../../../data/icons/gamepad.svg" alt="touch screen" width="50"/>

* cursor relative **range2**
* button **range01**

### touch screen

<img src="../../../data/icons/touch-screen.svg" alt="touch screen" width="50"/>

### osd button

### osd hat

### osd trigger

# credits

mouse, touchscreen
<div>Icons made by <a href="https://www.flaticon.com/authors/srip" title="srip">srip</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a></div>

keyboard, gamepad, joystick
<div>Icons made by <a href="https://www.freepik.com" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a></div>
