#include "click_processor.h"

#include "cursor_mac.h"
#include "cursor_linux.h"
#include "constants.h"
#include "face_to_cursor.h"

#include <iostream>

#ifdef __linux__
void ClickProcessor::handle_left_click(Display* display, const InputOperationsParams& params)
#elif __APPLE__
void ClickProcessor::handle_left_click(const FaceToCursor& face_to_cursor, const InputOperationsParams& params)
#endif
{
  // is_left_gesture_already_active keeps track if previous frame
  // gesture is active. A gesture can be defined as any facial
  // action like eye blink or mouth open.
  // I left this open ended in case anyone wants to switch
  // out different gestures

  // Ex. Left click can be controlled via left eye closing or
  // mouth opened.

  bool is_current_gesture_active = false;
  if(constants::USE_LEFT_EYE_BLINK) {
    is_current_gesture_active = params.is_left_eye_closed;
  } else {
    is_current_gesture_active = params.is_mouth_open;
  }

  if(
    m_is_left_gesture_already_active && 
    !is_current_gesture_active)
  {
    if(m_has_left_clicked) {
      #ifdef __linux__
      cursor::left_click_up(display);
      #elif __APPLE__
      cursor::left_click_up(face_to_cursor.get_curr_pointer_location());
      #endif

      // Do mouse up and reset
      std::cout << "Left click Up" << std::endl;
      m_has_left_clicked = false;
    }

    m_left_gesture_frames = 0;
  }

  if(
    !m_has_left_clicked &&
    m_is_left_gesture_already_active &&
    m_left_gesture_frames >= constants::LEFT_CLICK_FRAMES_THRESHOLD
  ) {
    // Click
    #ifdef __linux__
    cursor::left_click_down(display);
    #elif __APPLE__
    cursor::left_click_down(face_to_cursor.get_curr_pointer_location());
    #endif

    std::cout << "Left click Down" << std::endl;
    m_has_left_clicked = true;
  }
  
  if(is_current_gesture_active) {
    ++m_left_gesture_frames;
  }
  
  // Now set for next frame.
  m_is_left_gesture_already_active = is_current_gesture_active;

}

#ifdef __linux__
void ClickProcessor::handle_right_click(Display* display, const InputOperationsParams& params) 
#elif __APPLE__
void ClickProcessor::handle_right_click(const FaceToCursor& face_to_cursor, const InputOperationsParams& params)
#endif
{
  // is_right_gesture_already_active keeps track if previous frame
  // gesture is active. A gesture can be defined as any facial
  // action like eye blink or mouth open.
  // I left this open ended in case anyone wants to switch
  // out different gestures

  const bool is_current_gesture_active = params.is_right_eye_closed;
  if(
    m_is_right_gesture_already_active && 
    !is_current_gesture_active)
  {
    if(m_has_right_clicked) {
      #ifdef __linux__
      cursor::right_click_up(display);
      #elif __APPLE__
      cursor::right_click_up(face_to_cursor.get_curr_pointer_location());
      #endif

      // Do mouse up and reset
      std::cout << "Right click Up" << std::endl;
      m_has_right_clicked = false;
    }

    m_right_gesture_frames = 0;
  }

  if(
    !m_has_right_clicked &&
    m_is_right_gesture_already_active &&
    m_right_gesture_frames >= constants::RIGHT_CLICK_FRAMES_THRESHOLD
  ) {

    #ifdef __linux__
    cursor::right_click_down(display);
    #elif __APPLE__
    cursor::right_click_down(face_to_cursor.get_curr_pointer_location());
    #endif

    // Click
    std::cout << "Right click Down" << std::endl;
    m_has_right_clicked = true;
  }
  
  if(is_current_gesture_active) {
    // std::cout << m_right_gesture_frames << std::endl;
    ++m_right_gesture_frames;
  }
  
  // Now set for next frame.
  m_is_right_gesture_already_active = is_current_gesture_active;

}