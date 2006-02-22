#include "sdlutils.h"

using namespace tedomari::ui;

/** Converts an SDL mouse button value to a Button value */
Key tedomari::ui::sdl::sdlUtils_getButton(uint8 b)
{
  switch(b) {
    case SDL_BUTTON_LEFT:
      return K_MOUSELEFT;
    case SDL_BUTTON_RIGHT:
      return K_MOUSERIGHT;
    case SDL_BUTTON_MIDDLE:
      return K_MOUSEMIDDLE;
    case 4:
      return K_MOUSEWHEELUP;
    case 5:
      return K_MOUSEWHEELDOWN;
    default:
      Debug("unknown SDL mouse button number");
      return K_Unknown;
  }
}

/** Converts an SDLKey value to a Key value */
Key tedomari::ui::sdl::sdlUtils_getKey(SDLKey k)
{
  switch (k) {
    case SDLK_BACKSPACE:
      return K_BACKSPACE;
    case SDLK_TAB:
      return K_TAB;
    case SDLK_CLEAR:
      return K_CLEAR;
    case SDLK_RETURN:
      return K_RETURN;
    case SDLK_PAUSE:
      return K_PAUSE;
    case SDLK_ESCAPE:
      return K_ESCAPE;
    case SDLK_SPACE:
      return K_SPACE;
    case SDLK_EXCLAIM:
      return K_EXCLAIM;
    case SDLK_QUOTEDBL:
      return K_QUOTEDBL;
    case SDLK_HASH:
      return K_HASH;
    case SDLK_DOLLAR:
      return K_DOLLAR;
    case SDLK_AMPERSAND:
      return K_AMPERSAND;
    case SDLK_QUOTE:
      return K_QUOTE;
    case SDLK_LEFTPAREN:
      return K_LEFTPAREN;
    case SDLK_RIGHTPAREN:
      return K_RIGHTPAREN;
    case SDLK_ASTERISK:
      return K_ASTERISK;
    case SDLK_PLUS:
      return K_PLUS;
    case SDLK_COMMA:
      return K_COMMA;
    case SDLK_MINUS:
      return K_MINUS;
    case SDLK_PERIOD:
      return K_PERIOD;
    case SDLK_SLASH:
      return K_SLASH;
    case SDLK_0:
      return K_0;
    case SDLK_1:
      return K_1;
    case SDLK_2:
      return K_2;
    case SDLK_3:
      return K_3;
    case SDLK_4:
      return K_4;
    case SDLK_5:
      return K_5;
    case SDLK_6:
      return K_6;
    case SDLK_7:
      return K_7;
    case SDLK_8:
      return K_8;
    case SDLK_9:
      return K_9;
    case SDLK_COLON:
      return K_COLON;
    case SDLK_SEMICOLON:
      return K_SEMICOLON;
    case SDLK_LESS:
      return K_LESS;
    case SDLK_EQUALS:
      return K_EQUALS;
    case SDLK_GREATER:
      return K_GREATER;
    case SDLK_QUESTION:
      return K_QUESTION;
    case SDLK_AT:
      return K_AT;
    case SDLK_LEFTBRACKET:
      return K_LEFTBRACKET;
    case SDLK_BACKSLASH:
      return K_BACKSLASH;
    case SDLK_RIGHTBRACKET:
      return K_RIGHTBRACKET;
    case SDLK_CARET:
      return K_CARET;
    case SDLK_UNDERSCORE:
      return K_UNDERSCORE;
    case SDLK_BACKQUOTE:
      return K_BACKQUOTE;
    case SDLK_a:
      return K_a;
    case SDLK_b:
      return K_b;
    case SDLK_c:
      return K_c;
    case SDLK_d:
      return K_d;
    case SDLK_e:
      return K_e;
    case SDLK_f:
      return K_f;
    case SDLK_g:
      return K_g;
    case SDLK_h:
      return K_h;
    case SDLK_i:
      return K_i;
    case SDLK_j:
      return K_j;
    case SDLK_k:
      return K_k;
    case SDLK_l:
      return K_l;
    case SDLK_m:
      return K_m;
    case SDLK_n:
      return K_n;
    case SDLK_o:
      return K_o;
    case SDLK_p:
      return K_p;
    case SDLK_q:
      return K_q;
    case SDLK_r:
      return K_r;
    case SDLK_s:
      return K_s;
    case SDLK_t:
      return K_t;
    case SDLK_u:
      return K_u;
    case SDLK_v:
      return K_v;
    case SDLK_w:
      return K_w;
    case SDLK_x:
      return K_x;
    case SDLK_y:
      return K_y;
    case SDLK_z:
      return K_z;
    case SDLK_DELETE:
      return K_DELETE;
    case SDLK_KP0:
      return K_KP0;
    case SDLK_KP1:
      return K_KP1;
    case SDLK_KP2:
      return K_KP2;
    case SDLK_KP3:
      return K_KP3;
    case SDLK_KP4:
      return K_KP4;
    case SDLK_KP5:
      return K_KP5;
    case SDLK_KP6:
      return K_KP6;
    case SDLK_KP7:
      return K_KP7;
    case SDLK_KP8:
      return K_KP8;
    case SDLK_KP9:
      return K_KP9;
    case SDLK_KP_PERIOD:
      return K_KP_PERIOD;
    case SDLK_KP_DIVIDE:
      return K_KP_DIVIDE;
    case SDLK_KP_MULTIPLY:
      return K_KP_MULTIPLY;
    case SDLK_KP_MINUS:
      return K_KP_MINUS;
    case SDLK_KP_PLUS:
      return K_KP_PLUS;
    case SDLK_KP_ENTER:
      return K_KP_ENTER;
    case SDLK_KP_EQUALS:
      return K_KP_EQUALS;
    case SDLK_UP:
      return K_UP;
    case SDLK_DOWN:
      return K_DOWN;
    case SDLK_RIGHT:
      return K_RIGHT;
    case SDLK_LEFT:
      return K_LEFT;
    case SDLK_INSERT:
      return K_INSERT;
    case SDLK_HOME:
      return K_HOME;
    case SDLK_END:
      return K_END;
    case SDLK_PAGEUP:
      return K_PAGEUP;
    case SDLK_PAGEDOWN:
      return K_PAGEDOWN;
    case SDLK_F1:
      return K_F1;
    case SDLK_F2:
      return K_F2;
    case SDLK_F3:
      return K_F3;
    case SDLK_F4:
      return K_F4;
    case SDLK_F5:
      return K_F5;
    case SDLK_F6:
      return K_F6;
    case SDLK_F7:
      return K_F7;
    case SDLK_F8:
      return K_F8;
    case SDLK_F9:
      return K_F9;
    case SDLK_F10:
      return K_F10;
    case SDLK_F11:
      return K_F11;
    case SDLK_F12:
      return K_F12;
    case SDLK_F13:
      return K_F13;
    case SDLK_F14:
      return K_F14;
    case SDLK_F15:
      return K_F15;
    case SDLK_NUMLOCK:
      return K_NUMLOCK;
    case SDLK_CAPSLOCK:
      return K_CAPSLOCK;
    case SDLK_SCROLLOCK:
      return K_SCROLLOCK;
    case SDLK_RSHIFT:
      return K_RSHIFT;
    case SDLK_LSHIFT:
      return K_LSHIFT;
    case SDLK_RCTRL:
      return K_RCTRL;
    case SDLK_LCTRL:
      return K_LCTRL;
    case SDLK_RALT:
      return K_RALT;
    case SDLK_LALT:
      return K_LALT;
    case SDLK_RMETA:
      return K_RMETA;
    case SDLK_LMETA:
      return K_LMETA;
    case SDLK_LSUPER:
      return K_LSUPER;
    case SDLK_RSUPER:
      return K_RSUPER;
    case SDLK_MODE:
      return K_MODE;
    case SDLK_HELP:
      return K_HELP;
    case SDLK_PRINT:
      return K_PRINT;
    case SDLK_SYSREQ:
      return K_SYSREQ;
    case SDLK_BREAK:
      return K_BREAK;
    case SDLK_MENU:
      return K_MENU;
    case SDLK_POWER:
      return K_POWER;
    case SDLK_EURO:
      return K_EURO;
    default:
      Debug("Unexpected SDLKey: " << SDL_GetKeyName(k));
      return K_Unknown;
  }
}
