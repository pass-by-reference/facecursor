#include <string>

#include "constants.h"
#include "face_to_cursor.h"

FaceToCursor::FaceToCursor(
  int camera_width,
  int camera_height,
  int screen_width,
  int screen_height
) : m_camera_height{camera_height},
    m_camera_width{camera_width},
    m_screen_height{screen_height},
    m_screen_width{screen_width}
{}

std::pair<int, int> FaceToCursor::translate(const std::pair<int, int>& face_position) {
  return process_pointer_coords(initial_translate(face_position));
}

std::pair<int, int> FaceToCursor::initial_translate(const std::pair<int, int>& face_position) {
  int camera_x = face_position.first;
  int camera_y = face_position.second;

  if(camera_x == -1 || camera_y == -1) {
    return std::make_pair(0,0);
  }

  double c_x1 = ((double)2/8);
  double c_x2 = ((double)6/8);

  double c_y1 = ((double)3/8);
  double c_y2 = ((double)4/8);

  double m_x = m_screen_width / (c_x2 * m_camera_width - c_x1 * m_camera_width);
  double m_y = m_screen_height / (c_y2 * m_camera_height - c_y1 * m_camera_height);

  double x_constant = (m_screen_width * c_x1) / (c_x2 - c_x1);
  double y_constant = (m_screen_height * c_y1) / (c_y2 - c_y1);

  int screen_x = (m_x * (double)camera_x) - x_constant;
  int screen_y = (m_y * (double)camera_y) - y_constant;

  return std::make_pair(screen_x, screen_y);
}

std::pair<int, int> FaceToCursor::process_pointer_coords(const std::pair<int, int>& coords) {
  const int old_x = m_last_pointer_location.first;
  const int old_y = m_last_pointer_location.second;

  int new_x = coords.first;
  int new_y = coords.second;

  if(old_x == 0 && old_y == 0) {
    m_last_pointer_location = coords;
    return coords;
  }

  if(m_mouth_open_distance > constants::RESTING_MOUTH_DISTANCE) {
    new_y = new_y - (int)(1.2 * m_mouth_open_distance);
  }


  // Enable smooth cursor transition
  // If camera dimensions (width and height) is less than 
  // screen space (width and height) there loss of data when
  // translating from camera space to screen space.

  // To solve this issue, we take the average of the new and
  // old camera position to ensure some smoothing.
  std::pair<int, int> avg;
  if(old_x != 0 && old_y != 0) {
    const int avg_x = (new_x + old_x) / 2;
    const int avg_y = (new_y + old_y) / 2;

    avg = std::make_pair(avg_x, avg_y);
  }

  const bool is_new_x_within_range =
    is_within_range(old_x, new_x);
  const bool is_new_y_within_range =
    is_within_range(old_y, new_y);

  if(
    is_new_x_within_range && is_new_y_within_range    
  ){
    m_curr_location = m_last_pointer_location;
    return m_last_pointer_location;
  }

  m_curr_location = avg;
  m_last_pointer_location = avg;
  return coords;
}

void FaceToCursor::set_mouth_open_distance(const int distance) {
  m_mouth_open_distance = distance;
}

std::pair<int, int> FaceToCursor::get_last_pointer_location() const {
  return m_last_pointer_location;
}

std::pair<int, int> FaceToCursor::get_curr_pointer_location() const {
  return m_curr_location;
}

bool FaceToCursor::is_within_range(const int old, const int n) {
  int upper = old + m_in_range_pixels;
  int lower = old - m_in_range_pixels;
  return lower < n && n < upper;
}