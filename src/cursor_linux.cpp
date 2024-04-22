#ifdef __linux__

#include "constants.h"
#include "cursor_linux.h"

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
// Macro Complex declared in Xlib but also defined in opencv. Causes clash
// https://stackoverflow.com/questions/7109795/macro-definition-conflict
#undef Complex

namespace cursor {

void left_click_down(Display* display) {
  if(constants::SIMULATE_CLICK) {
    int ret_one = XTestFakeButtonEvent(display, Button1, true, CurrentTime);
    XFlush(display);
  }
}

void left_click_up(Display* display) {
  if(constants::SIMULATE_CLICK) {
    int ret_two = XTestFakeButtonEvent(display, Button1, false, CurrentTime);
    XFlush(display);
  }
}

void right_click_down(Display* display) {
  if(constants::SIMULATE_CLICK) {
    int ret_one = XTestFakeButtonEvent(display, Button3, true, CurrentTime);
    XFlush(display);
  }
}

void right_click_up(Display* display) {
  if(constants::SIMULATE_CLICK) {
    int ret_two = XTestFakeButtonEvent(display, Button3, false, CurrentTime);
    XFlush(display);
  }
}

void move_mouse(Display* display, Window window, int x, int y) {
  if(constants::SIMULATE_CURSOR_MOVE) {
    XWarpPointer(
      display, 
      window, 
      window,
      0,0,
      0,0,
      x,y
    );

    XFlush(display);
  }
}

} // namespace cursor

#endif 