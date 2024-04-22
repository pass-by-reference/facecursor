#ifndef FACE_TO_CURSOR_H_
#define FACE_TO_CURSOR_H_

class FaceToCursor {
public:
  FaceToCursor(
    int camera_width = 1024,
    int camera_height = 720,
    int screen_width = 1024,
    int screen_height = 720
  );

  std::pair<int, int> translate(const std::pair<int, int>& face_position);

  void set_mouth_open_distance(const int distance);
  std::pair<int, int> get_last_pointer_location() const;
  std::pair<int, int> get_curr_pointer_location() const;

private:
  
  std::pair<int, int> initial_translate(const std::pair<int, int>& face_position);
  std::pair<int, int> process_pointer_coords(const std::pair<int, int>& coords);

  bool is_within_range(const int old, const int n);

  std::pair<int, int> m_last_pointer_location = {0, 0};
  std::pair<int, int> m_curr_location = {0, 0};

  int m_in_range_pixels = 10;
  int m_mouth_open_distance = 0;

  int m_camera_width;
  int m_camera_height;
  int m_screen_width;
  int m_screen_height;
};

#endif // FACE_TO_CURSOR_H_