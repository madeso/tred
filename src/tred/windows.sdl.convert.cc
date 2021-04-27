#include "tred/windows.sdl.convert.h"

#include <cassert>


namespace sdl
{


glm::ivec2 get_hat_values(Uint8 hat)
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


input::keyboard_key to_keyboard_key(SDL_Keysym key)
{
    switch (key.sym)
    {
        case SDLK_RETURN: return input::keyboard_key::return_key;
        case SDLK_ESCAPE: return input::keyboard_key::escape;
        case SDLK_BACKSPACE: return input::keyboard_key::backspace;
        case SDLK_TAB: return input::keyboard_key::tab;
        case SDLK_SPACE: return input::keyboard_key::space;
        case SDLK_EXCLAIM: return input::keyboard_key::exclaim;
        case SDLK_QUOTEDBL: return input::keyboard_key::quotedbl;
        case SDLK_HASH: return input::keyboard_key::hash;
        case SDLK_PERCENT: return input::keyboard_key::percent;
        case SDLK_DOLLAR: return input::keyboard_key::dollar;
        case SDLK_AMPERSAND: return input::keyboard_key::ampersand;
        case SDLK_QUOTE: return input::keyboard_key::quote;
        case SDLK_LEFTPAREN: return input::keyboard_key::left_paren;
        case SDLK_RIGHTPAREN: return input::keyboard_key::right_paren;
        case SDLK_ASTERISK: return input::keyboard_key::asterix;
        case SDLK_PLUS: return input::keyboard_key::plus;
        case SDLK_COMMA: return input::keyboard_key::comma;
        case SDLK_MINUS: return input::keyboard_key::minus;
        case SDLK_PERIOD: return input::keyboard_key::period;
        case SDLK_SLASH: return input::keyboard_key::slash;
        case SDLK_0: return input::keyboard_key::num_0;
        case SDLK_1: return input::keyboard_key::num_1;
        case SDLK_2: return input::keyboard_key::num_2;
        case SDLK_3: return input::keyboard_key::num_3;
        case SDLK_4: return input::keyboard_key::num_4;
        case SDLK_5: return input::keyboard_key::num_5;
        case SDLK_6: return input::keyboard_key::num_6;
        case SDLK_7: return input::keyboard_key::num_7;
        case SDLK_8: return input::keyboard_key::num_8;
        case SDLK_9: return input::keyboard_key::num_9;
        case SDLK_COLON: return input::keyboard_key::colon;
        case SDLK_SEMICOLON: return input::keyboard_key::semicolon;
        case SDLK_LESS: return input::keyboard_key::less;
        case SDLK_EQUALS: return input::keyboard_key::equals;
        case SDLK_GREATER: return input::keyboard_key::greater;
        case SDLK_QUESTION: return input::keyboard_key::question;
        case SDLK_AT: return input::keyboard_key::at;
        case SDLK_LEFTBRACKET: return input::keyboard_key::left_bracket;
        case SDLK_BACKSLASH: return input::keyboard_key::backslash;
        case SDLK_RIGHTBRACKET: return input::keyboard_key::right_bracket;
        case SDLK_CARET: return input::keyboard_key::caret;
        case SDLK_UNDERSCORE: return input::keyboard_key::underscore;
        case SDLK_BACKQUOTE: return input::keyboard_key::backquote;
        case SDLK_a: return input::keyboard_key::a;
        case SDLK_b: return input::keyboard_key::b;
        case SDLK_c: return input::keyboard_key::c;
        case SDLK_d: return input::keyboard_key::d;
        case SDLK_e: return input::keyboard_key::e;
        case SDLK_f: return input::keyboard_key::f;
        case SDLK_g: return input::keyboard_key::g;
        case SDLK_h: return input::keyboard_key::h;
        case SDLK_i: return input::keyboard_key::i;
        case SDLK_j: return input::keyboard_key::j;
        case SDLK_k: return input::keyboard_key::k;
        case SDLK_l: return input::keyboard_key::l;
        case SDLK_m: return input::keyboard_key::m;
        case SDLK_n: return input::keyboard_key::n;
        case SDLK_o: return input::keyboard_key::o;
        case SDLK_p: return input::keyboard_key::p;
        case SDLK_q: return input::keyboard_key::q;
        case SDLK_r: return input::keyboard_key::r;
        case SDLK_s: return input::keyboard_key::s;
        case SDLK_t: return input::keyboard_key::t;
        case SDLK_u: return input::keyboard_key::u;
        case SDLK_v: return input::keyboard_key::v;
        case SDLK_w: return input::keyboard_key::w;
        case SDLK_x: return input::keyboard_key::x;
        case SDLK_y: return input::keyboard_key::y;
        case SDLK_z: return input::keyboard_key::z;
        case SDLK_CAPSLOCK: return input::keyboard_key::capslock;
        case SDLK_F1: return input::keyboard_key::f1;
        case SDLK_F2: return input::keyboard_key::f2;
        case SDLK_F3: return input::keyboard_key::f3;
        case SDLK_F4: return input::keyboard_key::f4;
        case SDLK_F5: return input::keyboard_key::f5;
        case SDLK_F6: return input::keyboard_key::f6;
        case SDLK_F7: return input::keyboard_key::f7;
        case SDLK_F8: return input::keyboard_key::f8;
        case SDLK_F9: return input::keyboard_key::f9;
        case SDLK_F10: return input::keyboard_key::f10;
        case SDLK_F11: return input::keyboard_key::f11;
        case SDLK_F12: return input::keyboard_key::f12;
        case SDLK_PRINTSCREEN: return input::keyboard_key::printscreen;
        case SDLK_SCROLLLOCK: return input::keyboard_key::scrolllock;
        case SDLK_PAUSE: return input::keyboard_key::pause;
        case SDLK_INSERT: return input::keyboard_key::insert;
        case SDLK_HOME: return input::keyboard_key::home;
        case SDLK_PAGEUP: return input::keyboard_key::pageup;
        case SDLK_DELETE: return input::keyboard_key::delete_key;
        case SDLK_END: return input::keyboard_key::end;
        case SDLK_PAGEDOWN: return input::keyboard_key::pagedown;
        case SDLK_RIGHT: return input::keyboard_key::right;
        case SDLK_LEFT: return input::keyboard_key::left;
        case SDLK_DOWN: return input::keyboard_key::down;
        case SDLK_UP: return input::keyboard_key::up;
        case SDLK_NUMLOCKCLEAR: return input::keyboard_key::numlock_clear;
        case SDLK_KP_DIVIDE: return input::keyboard_key::keypad_divide;
        case SDLK_KP_MULTIPLY: return input::keyboard_key::keypad_multiply;
        case SDLK_KP_MINUS: return input::keyboard_key::keypad_minus;
        case SDLK_KP_PLUS: return input::keyboard_key::keypad_plus;
        case SDLK_KP_ENTER: return input::keyboard_key::keypad_enter;
        case SDLK_KP_1: return input::keyboard_key::keypad_1;
        case SDLK_KP_2: return input::keyboard_key::keypad_2;
        case SDLK_KP_3: return input::keyboard_key::keypad_3;
        case SDLK_KP_4: return input::keyboard_key::keypad_4;
        case SDLK_KP_5: return input::keyboard_key::keypad_5;
        case SDLK_KP_6: return input::keyboard_key::keypad_6;
        case SDLK_KP_7: return input::keyboard_key::keypad_7;
        case SDLK_KP_8: return input::keyboard_key::keypad_8;
        case SDLK_KP_9: return input::keyboard_key::keypad_9;
        case SDLK_KP_0: return input::keyboard_key::keypad_0;
        case SDLK_KP_PERIOD: return input::keyboard_key::keypad_period;
        case SDLK_APPLICATION: return input::keyboard_key::application;
        case SDLK_POWER: return input::keyboard_key::power;
        case SDLK_KP_EQUALS: return input::keyboard_key::keypad_equals;
        case SDLK_F13: return input::keyboard_key::f13;
        case SDLK_F14: return input::keyboard_key::f14;
        case SDLK_F15: return input::keyboard_key::f15;
        case SDLK_F16: return input::keyboard_key::f16;
        case SDLK_F17: return input::keyboard_key::f17;
        case SDLK_F18: return input::keyboard_key::f18;
        case SDLK_F19: return input::keyboard_key::f19;
        case SDLK_F20: return input::keyboard_key::f20;
        case SDLK_F21: return input::keyboard_key::f21;
        case SDLK_F22: return input::keyboard_key::f22;
        case SDLK_F23: return input::keyboard_key::f23;
        case SDLK_F24: return input::keyboard_key::f24;
        case SDLK_EXECUTE: return input::keyboard_key::execute;
        case SDLK_HELP: return input::keyboard_key::help;
        case SDLK_MENU: return input::keyboard_key::menu;
        case SDLK_SELECT: return input::keyboard_key::select;
        case SDLK_STOP: return input::keyboard_key::stop;
        case SDLK_AGAIN: return input::keyboard_key::again;
        case SDLK_UNDO: return input::keyboard_key::undo;
        case SDLK_CUT: return input::keyboard_key::cut;
        case SDLK_COPY: return input::keyboard_key::copy;
        case SDLK_PASTE: return input::keyboard_key::paste;
        case SDLK_FIND: return input::keyboard_key::find;
        case SDLK_MUTE: return input::keyboard_key::mute;
        case SDLK_VOLUMEUP: return input::keyboard_key::volume_up;
        case SDLK_VOLUMEDOWN: return input::keyboard_key::volume_down;
        case SDLK_KP_COMMA: return input::keyboard_key::keypad_comma;
        case SDLK_KP_EQUALSAS400: return input::keyboard_key::keypad_equals_as_400;
        case SDLK_ALTERASE: return input::keyboard_key::alt_erase;
        case SDLK_SYSREQ: return input::keyboard_key::sysreq;
        case SDLK_CANCEL: return input::keyboard_key::cancel;
        case SDLK_CLEAR: return input::keyboard_key::clear;
        case SDLK_PRIOR: return input::keyboard_key::prior;
        case SDLK_RETURN2: return input::keyboard_key::second_return;
        case SDLK_SEPARATOR: return input::keyboard_key::separator;
        case SDLK_OUT: return input::keyboard_key::out;
        case SDLK_OPER: return input::keyboard_key::oper;
        case SDLK_CLEARAGAIN: return input::keyboard_key::clear_again;
        case SDLK_CRSEL: return input::keyboard_key::crsel;
        case SDLK_EXSEL: return input::keyboard_key::exsel;
        case SDLK_KP_00: return input::keyboard_key::keypad_00;
        case SDLK_KP_000: return input::keyboard_key::keypad_000;
        case SDLK_THOUSANDSSEPARATOR: return input::keyboard_key::thousand_separator;
        case SDLK_DECIMALSEPARATOR: return input::keyboard_key::decimal_separator;
        case SDLK_CURRENCYUNIT: return input::keyboard_key::currency_unit;
        case SDLK_CURRENCYSUBUNIT: return input::keyboard_key::currency_subunit;
        case SDLK_KP_LEFTPAREN: return input::keyboard_key::keypad_leftparen;
        case SDLK_KP_RIGHTPAREN: return input::keyboard_key::keypad_rightparen;
        case SDLK_KP_LEFTBRACE: return input::keyboard_key::keypad_leftbrace;
        case SDLK_KP_RIGHTBRACE: return input::keyboard_key::keypad_rightbrace;
        case SDLK_KP_TAB: return input::keyboard_key::keypad_tab;
        case SDLK_KP_BACKSPACE: return input::keyboard_key::keypad_backspace;
        case SDLK_KP_A: return input::keyboard_key::keypad_a;
        case SDLK_KP_B: return input::keyboard_key::keypad_b;
        case SDLK_KP_C: return input::keyboard_key::keypad_c;
        case SDLK_KP_D: return input::keyboard_key::keypad_d;
        case SDLK_KP_E: return input::keyboard_key::keypad_e;
        case SDLK_KP_F: return input::keyboard_key::keypad_f;
        case SDLK_KP_XOR: return input::keyboard_key::keypad_xor;
        case SDLK_KP_POWER: return input::keyboard_key::keypad_power;
        case SDLK_KP_PERCENT: return input::keyboard_key::keypad_percent;
        case SDLK_KP_LESS: return input::keyboard_key::keypad_less;
        case SDLK_KP_GREATER: return input::keyboard_key::keypad_greater;
        case SDLK_KP_AMPERSAND: return input::keyboard_key::keypad_ampersand;
        case SDLK_KP_DBLAMPERSAND: return input::keyboard_key::keypad_double_ampersand;
        case SDLK_KP_VERTICALBAR: return input::keyboard_key::keypad_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR: return input::keyboard_key::keypad_double_verticle_bar;
        case SDLK_KP_COLON: return input::keyboard_key::keypad_colon;
        case SDLK_KP_HASH: return input::keyboard_key::keypad_hash;
        case SDLK_KP_SPACE: return input::keyboard_key::keypad_space;
        case SDLK_KP_AT: return input::keyboard_key::keypad_at;
        case SDLK_KP_EXCLAM: return input::keyboard_key::keypad_exclam;
        case SDLK_KP_MEMSTORE: return input::keyboard_key::keypad_mem_store;
        case SDLK_KP_MEMRECALL: return input::keyboard_key::keypad_mem_recall;
        case SDLK_KP_MEMCLEAR: return input::keyboard_key::keypad_mem_clear;
        case SDLK_KP_MEMADD: return input::keyboard_key::keypad_mem_add;
        case SDLK_KP_MEMSUBTRACT: return input::keyboard_key::keypad_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return input::keyboard_key::keypad_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return input::keyboard_key::keypad_mem_divide;
        case SDLK_KP_PLUSMINUS: return input::keyboard_key::keypad_plus_minus;
        case SDLK_KP_CLEAR: return input::keyboard_key::keypad_clear;
        case SDLK_KP_CLEARENTRY: return input::keyboard_key::keypad_clear_entry;
        case SDLK_KP_BINARY: return input::keyboard_key::keypad_binary;
        case SDLK_KP_OCTAL: return input::keyboard_key::keypad_octal;
        case SDLK_KP_DECIMAL: return input::keyboard_key::keypad_decimal;
        case SDLK_KP_HEXADECIMAL: return input::keyboard_key::keypad_hexadecimal;
        case SDLK_LCTRL: return input::keyboard_key::ctrl_left;
        case SDLK_LSHIFT: return input::keyboard_key::shift_left;
        case SDLK_LALT: return input::keyboard_key::alt_left;
        case SDLK_LGUI: return input::keyboard_key::gui_left;
        case SDLK_RCTRL: return input::keyboard_key::ctrl_right;
        case SDLK_RSHIFT: return input::keyboard_key::shift_right;
        case SDLK_RALT: return input::keyboard_key::alt_right;
        case SDLK_RGUI: return input::keyboard_key::gui_right;
        case SDLK_MODE: return input::keyboard_key::mode;
        case SDLK_AUDIONEXT: return input::keyboard_key::audio_next;
        case SDLK_AUDIOPREV: return input::keyboard_key::audio_prev;
        case SDLK_AUDIOSTOP: return input::keyboard_key::audio_stop;
        case SDLK_AUDIOPLAY: return input::keyboard_key::audio_play;
        case SDLK_AUDIOMUTE: return input::keyboard_key::audio_mute;
        case SDLK_MEDIASELECT: return input::keyboard_key::mediaselect;
        case SDLK_WWW: return input::keyboard_key::www;
        case SDLK_MAIL: return input::keyboard_key::mail;
        case SDLK_CALCULATOR: return input::keyboard_key::calculator;
        case SDLK_COMPUTER: return input::keyboard_key::computer;
        case SDLK_AC_SEARCH: return input::keyboard_key::ac_search;
        case SDLK_AC_HOME: return input::keyboard_key::ac_home;
        case SDLK_AC_BACK: return input::keyboard_key::ac_back;
        case SDLK_AC_FORWARD: return input::keyboard_key::ac_forward;
        case SDLK_AC_STOP: return input::keyboard_key::ac_stop;
        case SDLK_AC_REFRESH: return input::keyboard_key::ac_refresh;
        case SDLK_AC_BOOKMARKS: return input::keyboard_key::ac_bookmarks;
        case SDLK_BRIGHTNESSDOWN: return input::keyboard_key::brightness_down;
        case SDLK_BRIGHTNESSUP: return input::keyboard_key::brightness_up;
        case SDLK_DISPLAYSWITCH: return input::keyboard_key::displayswitch;
        case SDLK_KBDILLUMTOGGLE: return input::keyboard_key::keyboard_illum_toggle;
        case SDLK_KBDILLUMDOWN: return input::keyboard_key::keyboard_illum_down;
        case SDLK_KBDILLUMUP: return input::keyboard_key::keyboard_illum_up;
        case SDLK_EJECT: return input::keyboard_key::eject;
        case SDLK_SLEEP: return input::keyboard_key::sleep;
        default:
            assert(false && key.sym && "Invalid keyboard button");
            return input::keyboard_key::invalid;
    }
}


input::mouse_button to_mouse_button(Uint8 mb)
{
    switch (mb)
    {
        case SDL_BUTTON_LEFT: return input::mouse_button::left;
        case SDL_BUTTON_MIDDLE: return input::mouse_button::middle;
        case SDL_BUTTON_RIGHT: return input::mouse_button::right;
        case SDL_BUTTON_X1: return input::mouse_button::x1;
        case SDL_BUTTON_X2: return input::mouse_button::x2;
        default:
            assert(false && mb && "Invalid mouse button");
            return input::mouse_button::invalid;
    }
}


input::gamecontroller_button to_controller_button(SDL_GameControllerButton b)
{
    switch(b)
    {
        case SDL_CONTROLLER_BUTTON_A: return input::gamecontroller_button::a;
        case SDL_CONTROLLER_BUTTON_B: return input::gamecontroller_button::b;
        case SDL_CONTROLLER_BUTTON_X: return input::gamecontroller_button::x;
        case SDL_CONTROLLER_BUTTON_Y: return input::gamecontroller_button::y;
        case SDL_CONTROLLER_BUTTON_BACK: return input::gamecontroller_button::back;
        case SDL_CONTROLLER_BUTTON_GUIDE: return input::gamecontroller_button::guide;
        case SDL_CONTROLLER_BUTTON_START: return input::gamecontroller_button::start;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return input::gamecontroller_button::stick_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return input::gamecontroller_button::stick_right;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return input::gamecontroller_button::shoulder_left;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return input::gamecontroller_button::shoulder_right;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return input::gamecontroller_button::dpad_up;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return input::gamecontroller_button::dpad_down;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return input::gamecontroller_button::dpad_left;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return input::gamecontroller_button::dpad_right;
// todo(Gustav): when were theese introduced? not in 2.0.8 at least but I can't seem to find when atm
#if SDL_VERSION_ATLEAST(2, 0, 9)
        case SDL_CONTROLLER_BUTTON_MISC1: return input::GamecontrollerButton::MISC1;
        case SDL_CONTROLLER_BUTTON_PADDLE1: return input::GamecontrollerButton::PADDLE1;
        case SDL_CONTROLLER_BUTTON_PADDLE2: return input::GamecontrollerButton::PADDLE2;
        case SDL_CONTROLLER_BUTTON_PADDLE3: return input::GamecontrollerButton::PADDLE3;
        case SDL_CONTROLLER_BUTTON_PADDLE4: return input::GamecontrollerButton::PADDLE4;
        case SDL_CONTROLLER_BUTTON_TOUCHPAD: return input::GamecontrollerButton::TOUCHPAD;
#endif
        default:
        return input::gamecontroller_button::invalid;
    }
}

input::gamecontroller_button to_controller_button(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return input::gamecontroller_button::trigger_left;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return input::gamecontroller_button::trigger_right;
        default:
        return input::gamecontroller_button::invalid;
    }
}

input::gamecontroller_axis to_controller_axis(SDL_GameControllerAxis a)
{
    switch(a)
    {
        case SDL_CONTROLLER_AXIS_LEFTX: return input::gamecontroller_axis::left_x;
        case SDL_CONTROLLER_AXIS_LEFTY: return input::gamecontroller_axis::left_y;
        case SDL_CONTROLLER_AXIS_RIGHTX: return input::gamecontroller_axis::right_x;
        case SDL_CONTROLLER_AXIS_RIGHTY: return input::gamecontroller_axis::right_y;
        default: return input::gamecontroller_axis::invalid;
    }
}


}
