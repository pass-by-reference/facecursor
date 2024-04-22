#ifndef CLICK_PROCESSOR_H_
#define CLICK_PROCESSOR_H_

#include <utility>
#include "face_to_cursor.h"

#ifdef __linux__
#include <X11/Xlib.h>
// Macro Complex declared in Xlib but also defined in opencv. Causes clash
// https://stackoverflow.com/questions/7109795/macro-definition-conflict
#undef Complex
#endif

struct InputOperationsParams {
  std::pair<int, int> face_position_coords;
  bool is_left_eye_closed;
  bool is_right_eye_closed;
  bool is_mouth_open;
};

class ClickProcessor {
public:
#ifdef __linux__
  void handle_left_click(Display* display, const InputOperationsParams& params);
  void handle_right_click(Display* display, const InputOperationsParams& params);
#elif __APPLE__
  void handle_left_click(const FaceToCursor& face_to_cursor, const InputOperationsParams& params);
  void handle_right_click(const FaceToCursor& face_to_cursor, const InputOperationsParams& params);
#endif

private:
  bool m_is_right_gesture_already_active = false;
  bool m_has_right_clicked = false;
  int m_right_gesture_frames = 0;

  bool m_is_left_gesture_already_active = false;
  bool m_has_left_clicked = false;
  int m_left_gesture_frames = 0;

};

#endif // CLICK_PROCESSOR_H_