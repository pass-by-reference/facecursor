#ifndef CURSOR_LINUX_H_
#define CURSOR_LINUX_H_

#ifdef __linux__

#include <X11/Xlib.h>
// Macro Complex declared in Xlib but also defined in opencv. Causes clash
// https://stackoverflow.com/questions/7109795/macro-definition-conflict
#undef Complex

namespace cursor {
void move_mouse(Display* display, Window window, int x, int y);
void left_click_down(Display* display);
void left_click_up(Display* display);
void right_click_up(Display* display);
void right_click_down(Display* display);
}

#endif

#endif