#ifndef CURSOR_MAC_H_
#define CURSOR_MAC_H_

#ifdef __APPLE__

namespace cursor {

void move_mouse(int x, int y);
void left_click_down(const std::pair<int, int>& curr_location);
void left_click_up(const std::pair<int, int>& curr_location);
void right_click_up(const std::pair<int, int>& curr_location);
void right_click_down(const std::pair<int, int>& curr_location);
}

#endif

#endif // CURSOR_MAC_H_