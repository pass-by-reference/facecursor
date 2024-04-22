#ifdef __APPLE__

#include <utility>

#include "constants.h"
#include "cursor_mac.h"
// MacOS specific lib for controlling mouse and keys
#include <ApplicationServices/ApplicationServices.h>

namespace cursor {

void move_mouse(int x, int y) {
  if(constants::SIMULATE_CURSOR_MOVE) {
    CGDirectDisplayID display_id = CGMainDisplayID();

    CGPoint point = {
      .x=static_cast<CGFloat>(x),
      .y=static_cast<CGFloat>(y)
    };

    CGDisplayMoveCursorToPoint(display_id, point);
  }
}

void left_click_down(const std::pair<int, int>& curr_location) {
  if(constants::SIMULATE_CLICK) {
    CGPoint point = {
      .x=static_cast<CGFloat>(curr_location.first),
      .y=static_cast<CGFloat>(curr_location.second)
    };

    // std::cout << point.x << " " << point.y << std::endl;

    auto mouse_down_event = CGEventCreateMouseEvent(
      NULL,
      CGEventType::kCGEventLeftMouseDown,
      point,
      CGMouseButton::kCGMouseButtonLeft
    );

    CGEventPost (kCGSessionEventTap, mouse_down_event);
  }
}

void left_click_up(const std::pair<int, int>& curr_location) {

  if(constants::SIMULATE_CLICK) {
    CGPoint point = {
      .x=static_cast<CGFloat>(curr_location.first),
      .y=static_cast<CGFloat>(curr_location.second)
    };

    auto mouse_up_event = CGEventCreateMouseEvent(
      NULL,
      CGEventType::kCGEventLeftMouseUp,
      point,
      CGMouseButton::kCGMouseButtonLeft
    );

    CGEventPost (kCGSessionEventTap, mouse_up_event );
  }
}

void right_click_up(const std::pair<int, int>& curr_location) {

}

void right_click_down(const std::pair<int, int>& curr_location) {

}

} // namespace macos

#endif