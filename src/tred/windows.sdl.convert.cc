#include "tred/windows.sdl.convert.h"

#include <cassert>


namespace sdl
{


glm::ivec2 GetHatValues(Uint8 hat)
{
    switch (hat)
    {
        case SDL_HAT_LEFTUP: return glm::ivec2(-1, 1);
        case SDL_HAT_UP: return glm::ivec2(0, 1);
        case SDL_HAT_RIGHTUP: return glm::ivec2(1, 1);
        case SDL_HAT_LEFT: return glm::ivec2(-1, 0);
        case SDL_HAT_CENTERED: return glm::ivec2(0, 0);
        case SDL_HAT_RIGHT: return glm::ivec2(1, 0);
        case SDL_HAT_LEFTDOWN: return glm::ivec2(-1, -1);
        case SDL_HAT_DOWN: return glm::ivec2(0, -1);
        case SDL_HAT_RIGHTDOWN: return glm::ivec2(1, -1);
        default:
            assert(false && "Invalid hat value");
            return glm::ivec2(0, 0);
    }
}


input::Key ToKey(SDL_Keysym key)
{
    switch (key.sym)
    {
        case SDLK_RETURN: return input::Key::RETURN;
        case SDLK_ESCAPE: return input::Key::ESCAPE;
        case SDLK_BACKSPACE: return input::Key::BACKSPACE;
        case SDLK_TAB: return input::Key::TAB;
        case SDLK_SPACE: return input::Key::SPACE;
        case SDLK_EXCLAIM: return input::Key::EXCLAIM;
        case SDLK_QUOTEDBL: return input::Key::QUOTEDBL;
        case SDLK_HASH: return input::Key::HASH;
        case SDLK_PERCENT: return input::Key::PERCENT;
        case SDLK_DOLLAR: return input::Key::DOLLAR;
        case SDLK_AMPERSAND: return input::Key::AMPERSAND;
        case SDLK_QUOTE: return input::Key::QUOTE;
        case SDLK_LEFTPAREN: return input::Key::LEFT_PAREN;
        case SDLK_RIGHTPAREN: return input::Key::RIGHT_PAREN;
        case SDLK_ASTERISK: return input::Key::ASTERIX;
        case SDLK_PLUS: return input::Key::PLUS;
        case SDLK_COMMA: return input::Key::COMMA;
        case SDLK_MINUS: return input::Key::MINUS;
        case SDLK_PERIOD: return input::Key::PERIOD;
        case SDLK_SLASH: return input::Key::SLASH;
        case SDLK_0: return input::Key::NUM_0;
        case SDLK_1: return input::Key::NUM_1;
        case SDLK_2: return input::Key::NUM_2;
        case SDLK_3: return input::Key::NUM_3;
        case SDLK_4: return input::Key::NUM_4;
        case SDLK_5: return input::Key::NUM_5;
        case SDLK_6: return input::Key::NUM_6;
        case SDLK_7: return input::Key::NUM_7;
        case SDLK_8: return input::Key::NUM_8;
        case SDLK_9: return input::Key::NUM_9;
        case SDLK_COLON: return input::Key::COLON;
        case SDLK_SEMICOLON: return input::Key::SEMICOLON;
        case SDLK_LESS: return input::Key::LESS;
        case SDLK_EQUALS: return input::Key::EQUALS;
        case SDLK_GREATER: return input::Key::GREATER;
        case SDLK_QUESTION: return input::Key::QUESTION;
        case SDLK_AT: return input::Key::AT;
        case SDLK_LEFTBRACKET: return input::Key::LEFT_BRACKET;
        case SDLK_BACKSLASH: return input::Key::BACKSLASH;
        case SDLK_RIGHTBRACKET: return input::Key::RIGHT_BRACKET;
        case SDLK_CARET: return input::Key::CARET;
        case SDLK_UNDERSCORE: return input::Key::UNDERSCORE;
        case SDLK_BACKQUOTE: return input::Key::BACKQUOTE;
        case SDLK_a: return input::Key::A;
        case SDLK_b: return input::Key::B;
        case SDLK_c: return input::Key::C;
        case SDLK_d: return input::Key::D;
        case SDLK_e: return input::Key::E;
        case SDLK_f: return input::Key::F;
        case SDLK_g: return input::Key::G;
        case SDLK_h: return input::Key::H;
        case SDLK_i: return input::Key::I;
        case SDLK_j: return input::Key::J;
        case SDLK_k: return input::Key::K;
        case SDLK_l: return input::Key::L;
        case SDLK_m: return input::Key::M;
        case SDLK_n: return input::Key::N;
        case SDLK_o: return input::Key::O;
        case SDLK_p: return input::Key::P;
        case SDLK_q: return input::Key::Q;
        case SDLK_r: return input::Key::R;
        case SDLK_s: return input::Key::S;
        case SDLK_t: return input::Key::T;
        case SDLK_u: return input::Key::U;
        case SDLK_v: return input::Key::V;
        case SDLK_w: return input::Key::W;
        case SDLK_x: return input::Key::X;
        case SDLK_y: return input::Key::Y;
        case SDLK_z: return input::Key::Z;
        case SDLK_CAPSLOCK: return input::Key::CAPSLOCK;
        case SDLK_F1: return input::Key::F1;
        case SDLK_F2: return input::Key::F2;
        case SDLK_F3: return input::Key::F3;
        case SDLK_F4: return input::Key::F4;
        case SDLK_F5: return input::Key::F5;
        case SDLK_F6: return input::Key::F6;
        case SDLK_F7: return input::Key::F7;
        case SDLK_F8: return input::Key::F8;
        case SDLK_F9: return input::Key::F9;
        case SDLK_F10: return input::Key::F10;
        case SDLK_F11: return input::Key::F11;
        case SDLK_F12: return input::Key::F12;
        case SDLK_PRINTSCREEN: return input::Key::PRINTSCREEN;
        case SDLK_SCROLLLOCK: return input::Key::SCROLLLOCK;
        case SDLK_PAUSE: return input::Key::PAUSE;
        case SDLK_INSERT: return input::Key::INSERT;
        case SDLK_HOME: return input::Key::HOME;
        case SDLK_PAGEUP: return input::Key::PAGEUP;
        case SDLK_DELETE: return input::Key::DELETE;
        case SDLK_END: return input::Key::END;
        case SDLK_PAGEDOWN: return input::Key::PAGEDOWN;
        case SDLK_RIGHT: return input::Key::RIGHT;
        case SDLK_LEFT: return input::Key::LEFT;
        case SDLK_DOWN: return input::Key::DOWN;
        case SDLK_UP: return input::Key::UP;
        case SDLK_NUMLOCKCLEAR: return input::Key::NUMLOCK_CLEAR;
        case SDLK_KP_DIVIDE: return input::Key::KEYPAD_DIVIDE;
        case SDLK_KP_MULTIPLY: return input::Key::KEYPAD_MULTIPLY;
        case SDLK_KP_MINUS: return input::Key::KEYPAD_MINUS;
        case SDLK_KP_PLUS: return input::Key::KEYPAD_PLUS;
        case SDLK_KP_ENTER: return input::Key::KEYPAD_ENTER;
        case SDLK_KP_1: return input::Key::KEYPAD_1;
        case SDLK_KP_2: return input::Key::KEYPAD_2;
        case SDLK_KP_3: return input::Key::KEYPAD_3;
        case SDLK_KP_4: return input::Key::KEYPAD_4;
        case SDLK_KP_5: return input::Key::KEYPAD_5;
        case SDLK_KP_6: return input::Key::KEYPAD_6;
        case SDLK_KP_7: return input::Key::KEYPAD_7;
        case SDLK_KP_8: return input::Key::KEYPAD_8;
        case SDLK_KP_9: return input::Key::KEYPAD_9;
        case SDLK_KP_0: return input::Key::KEYPAD_0;
        case SDLK_KP_PERIOD: return input::Key::KEYPAD_PERIOD;
        case SDLK_APPLICATION: return input::Key::APPLICATION;
        case SDLK_POWER: return input::Key::POWER;
        case SDLK_KP_EQUALS: return input::Key::KEYPAD_EQUALS;
        case SDLK_F13: return input::Key::F13;
        case SDLK_F14: return input::Key::F14;
        case SDLK_F15: return input::Key::F15;
        case SDLK_F16: return input::Key::F16;
        case SDLK_F17: return input::Key::F17;
        case SDLK_F18: return input::Key::F18;
        case SDLK_F19: return input::Key::F19;
        case SDLK_F20: return input::Key::F20;
        case SDLK_F21: return input::Key::F21;
        case SDLK_F22: return input::Key::F22;
        case SDLK_F23: return input::Key::F23;
        case SDLK_F24: return input::Key::F24;
        case SDLK_EXECUTE: return input::Key::EXECUTE;
        case SDLK_HELP: return input::Key::HELP;
        case SDLK_MENU: return input::Key::MENU;
        case SDLK_SELECT: return input::Key::SELECT;
        case SDLK_STOP: return input::Key::STOP;
        case SDLK_AGAIN: return input::Key::AGAIN;
        case SDLK_UNDO: return input::Key::UNDO;
        case SDLK_CUT: return input::Key::CUT;
        case SDLK_COPY: return input::Key::COPY;
        case SDLK_PASTE: return input::Key::PASTE;
        case SDLK_FIND: return input::Key::FIND;
        case SDLK_MUTE: return input::Key::MUTE;
        case SDLK_VOLUMEUP: return input::Key::VOLUME_UP;
        case SDLK_VOLUMEDOWN: return input::Key::VOLUME_DOWN;
        case SDLK_KP_COMMA: return input::Key::KEYPAD_COMMA;
        case SDLK_KP_EQUALSAS400: return input::Key::KEYPAD_EQUALS_AS_400;
        case SDLK_ALTERASE: return input::Key::ALT_ERASE;
        case SDLK_SYSREQ: return input::Key::SYSREQ;
        case SDLK_CANCEL: return input::Key::CANCEL;
        case SDLK_CLEAR: return input::Key::CLEAR;
        case SDLK_PRIOR: return input::Key::PRIOR;
        case SDLK_RETURN2: return input::Key::SECOND_RETURN;
        case SDLK_SEPARATOR: return input::Key::SEPARATOR;
        case SDLK_OUT: return input::Key::OUT;
        case SDLK_OPER: return input::Key::OPER;
        case SDLK_CLEARAGAIN: return input::Key::CLEAR_AGAIN;
        case SDLK_CRSEL: return input::Key::CRSEL;
        case SDLK_EXSEL: return input::Key::EXSEL;
        case SDLK_KP_00: return input::Key::KEYPAD_00;
        case SDLK_KP_000: return input::Key::KEYPAD_000;
        case SDLK_THOUSANDSSEPARATOR: return input::Key::THOUSANDSEPARATOR;
        case SDLK_DECIMALSEPARATOR: return input::Key::DECIMALSEPARATOR;
        case SDLK_CURRENCYUNIT: return input::Key::CURRENCY_UNIT;
        case SDLK_CURRENCYSUBUNIT: return input::Key::CURRENCY_SUBUNIT;
        case SDLK_KP_LEFTPAREN: return input::Key::KEYPAD_LEFTPAREN;
        case SDLK_KP_RIGHTPAREN: return input::Key::KEYPAD_RIGHTPAREN;
        case SDLK_KP_LEFTBRACE: return input::Key::KEYPAD_LEFTBRACE;
        case SDLK_KP_RIGHTBRACE: return input::Key::KEYPAD_RIGHTBRACE;
        case SDLK_KP_TAB: return input::Key::KEYPAD_TAB;
        case SDLK_KP_BACKSPACE: return input::Key::KEYPAD_BACKSPACE;
        case SDLK_KP_A: return input::Key::KEYPAD_A;
        case SDLK_KP_B: return input::Key::KEYPAD_B;
        case SDLK_KP_C: return input::Key::KEYPAD_C;
        case SDLK_KP_D: return input::Key::KEYPAD_D;
        case SDLK_KP_E: return input::Key::KEYPAD_E;
        case SDLK_KP_F: return input::Key::KEYPAD_F;
        case SDLK_KP_XOR: return input::Key::KEYPAD_XOR;
        case SDLK_KP_POWER: return input::Key::KEYPAD_POWER;
        case SDLK_KP_PERCENT: return input::Key::KEYPAD_PERCENT;
        case SDLK_KP_LESS: return input::Key::KEYPAD_LESS;
        case SDLK_KP_GREATER: return input::Key::KEYPAD_GREATER;
        case SDLK_KP_AMPERSAND: return input::Key::KEYPAD_AMPERSAND;
        case SDLK_KP_DBLAMPERSAND: return input::Key::KEYPAD_DOUBLE_AMPERSAND;
        case SDLK_KP_VERTICALBAR: return input::Key::KEYPAD_VERTICAL_BAR;
        case SDLK_KP_DBLVERTICALBAR: return input::Key::KEYPAD_DOUBLE_VERTICLE_BAR;
        case SDLK_KP_COLON: return input::Key::KEYPAD_COLON;
        case SDLK_KP_HASH: return input::Key::KEYPAD_HASH;
        case SDLK_KP_SPACE: return input::Key::KEYPAD_SPACE;
        case SDLK_KP_AT: return input::Key::KEYPAD_AT;
        case SDLK_KP_EXCLAM: return input::Key::KEYPAD_EXCLAM;
        case SDLK_KP_MEMSTORE: return input::Key::KEYPAD_MEM_STORE;
        case SDLK_KP_MEMRECALL: return input::Key::KEYPAD_MEM_RECALL;
        case SDLK_KP_MEMCLEAR: return input::Key::KEYPAD_MEM_CLEAR;
        case SDLK_KP_MEMADD: return input::Key::KEYPAD_MEM_ADD;
        case SDLK_KP_MEMSUBTRACT: return input::Key::KEYPAD_MEM_SUBTRACT;
        case SDLK_KP_MEMMULTIPLY: return input::Key::KEYPAD_MEM_MULTIPLY;
        case SDLK_KP_MEMDIVIDE: return input::Key::KEYPAD_MEM_DIVIDE;
        case SDLK_KP_PLUSMINUS: return input::Key::KEYPAD_PLUS_MINUS;
        case SDLK_KP_CLEAR: return input::Key::KEYPAD_CLEAR;
        case SDLK_KP_CLEARENTRY: return input::Key::KEYPAD_CLEAR_ENTRY;
        case SDLK_KP_BINARY: return input::Key::KEYPAD_BINARY;
        case SDLK_KP_OCTAL: return input::Key::KEYPAD_OCTAL;
        case SDLK_KP_DECIMAL: return input::Key::KEYPAD_DECIMAL;
        case SDLK_KP_HEXADECIMAL: return input::Key::KEYPAD_HEXADECIMAL;
        case SDLK_LCTRL: return input::Key::CTRL_LEFT;
        case SDLK_LSHIFT: return input::Key::SHIFT_LEFT;
        case SDLK_LALT: return input::Key::ALT_LEFT;
        case SDLK_LGUI: return input::Key::GUI_LEFT;
        case SDLK_RCTRL: return input::Key::CTRL_RIGHT;
        case SDLK_RSHIFT: return input::Key::SHIFT_RIGHT;
        case SDLK_RALT: return input::Key::ALT_RIGHT;
        case SDLK_RGUI: return input::Key::GUI_RIGHT;
        case SDLK_MODE: return input::Key::MODE;
        case SDLK_AUDIONEXT: return input::Key::AUDIO_NEXT;
        case SDLK_AUDIOPREV: return input::Key::AUDIO_PREV;
        case SDLK_AUDIOSTOP: return input::Key::AUDIO_STOP;
        case SDLK_AUDIOPLAY: return input::Key::AUDIO_PLAY;
        case SDLK_AUDIOMUTE: return input::Key::AUDIO_MUTE;
        case SDLK_MEDIASELECT: return input::Key::MEDIASELECT;
        case SDLK_WWW: return input::Key::WWW;
        case SDLK_MAIL: return input::Key::MAIL;
        case SDLK_CALCULATOR: return input::Key::CALCULATOR;
        case SDLK_COMPUTER: return input::Key::COMPUTER;
        case SDLK_AC_SEARCH: return input::Key::AC_SEARCH;
        case SDLK_AC_HOME: return input::Key::AC_HOME;
        case SDLK_AC_BACK: return input::Key::AC_BACK;
        case SDLK_AC_FORWARD: return input::Key::AC_FORWARD;
        case SDLK_AC_STOP: return input::Key::AC_STOP;
        case SDLK_AC_REFRESH: return input::Key::AC_REFRESH;
        case SDLK_AC_BOOKMARKS: return input::Key::AC_BOOKMARKS;
        case SDLK_BRIGHTNESSDOWN: return input::Key::BRIGHTNESS_DOWN;
        case SDLK_BRIGHTNESSUP: return input::Key::BRIGHTNESS_UP;
        case SDLK_DISPLAYSWITCH: return input::Key::DISPLAYSWITCH;
        case SDLK_KBDILLUMTOGGLE: return input::Key::KEYBOARD_ILLUM_TOGGLE;
        case SDLK_KBDILLUMDOWN: return input::Key::KEYBOARD_ILLUM_DOWN;
        case SDLK_KBDILLUMUP: return input::Key::KEYBOARD_ILLUM_UP;
        case SDLK_EJECT: return input::Key::EJECT;
        case SDLK_SLEEP: return input::Key::SLEEP;
        default:
            assert(false && key.sym && "Invalid keyboard button");
            return input::Key::INVALID;
    }
}


input::MouseButton ToMouseButton(Uint8 mb)
{
    switch (mb)
    {
        case SDL_BUTTON_LEFT: return input::MouseButton::LEFT;
        case SDL_BUTTON_MIDDLE: return input::MouseButton::MIDDLE;
        case SDL_BUTTON_RIGHT: return input::MouseButton::RIGHT;
        case SDL_BUTTON_X1: return input::MouseButton::X1;
        case SDL_BUTTON_X2: return input::MouseButton::X2;
        default:
            assert(false && mb && "Invalid mouse button");
            return input::MouseButton::INVALID;
    }
}


input::GamecontrollerButton ToButton(SDL_GameControllerButton b)
{
    switch(b)
    {
        case SDL_CONTROLLER_BUTTON_A: return input::GamecontrollerButton::A;
        case SDL_CONTROLLER_BUTTON_B: return input::GamecontrollerButton::B;
        case SDL_CONTROLLER_BUTTON_X: return input::GamecontrollerButton::X;
        case SDL_CONTROLLER_BUTTON_Y: return input::GamecontrollerButton::Y;
        case SDL_CONTROLLER_BUTTON_BACK: return input::GamecontrollerButton::BACK;
        case SDL_CONTROLLER_BUTTON_GUIDE: return input::GamecontrollerButton::GUIDE;
        case SDL_CONTROLLER_BUTTON_START: return input::GamecontrollerButton::START;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return input::GamecontrollerButton::LEFTSTICK;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return input::GamecontrollerButton::RIGHTSTICK;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return input::GamecontrollerButton::LEFTSHOULDER;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return input::GamecontrollerButton::RIGHTSHOULDER;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return input::GamecontrollerButton::DPAD_UP;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return input::GamecontrollerButton::DPAD_DOWN;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return input::GamecontrollerButton::DPAD_LEFT;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return input::GamecontrollerButton::DPAD_RIGHT;
        case SDL_CONTROLLER_BUTTON_MISC1: return input::GamecontrollerButton::MISC1;
        case SDL_CONTROLLER_BUTTON_PADDLE1: return input::GamecontrollerButton::PADDLE1;
        case SDL_CONTROLLER_BUTTON_PADDLE2: return input::GamecontrollerButton::PADDLE2;
        case SDL_CONTROLLER_BUTTON_PADDLE3: return input::GamecontrollerButton::PADDLE3;
        case SDL_CONTROLLER_BUTTON_PADDLE4: return input::GamecontrollerButton::PADDLE4;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD: return input::GamecontrollerButton::TOUCHPAD;
        default:
        return input::GamecontrollerButton::INVALID;
    }
}

input::GamecontrollerButton ToButton(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return input::GamecontrollerButton::TRIGGER_LEFT;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return input::GamecontrollerButton::TRIGGER_RIGHT;
        default:
        return input::GamecontrollerButton::INVALID;
    }
}

input::GamecontrollerAxis ToAxis(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_LEFTX: return input::GamecontrollerAxis::LEFTX;
        case SDL_CONTROLLER_AXIS_LEFTY: return input::GamecontrollerAxis::LEFTY;
        case SDL_CONTROLLER_AXIS_RIGHTX: return input::GamecontrollerAxis::RIGHTX;
        case SDL_CONTROLLER_AXIS_RIGHTY: return input::GamecontrollerAxis::RIGHTY;
        default: return input::GamecontrollerAxis::INVALID;
    }
}


}
