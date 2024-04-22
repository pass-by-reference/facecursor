#ifndef LANDMARK_H_
#define LANDMARK_H_

#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>

struct DetectMouthReturn {
  int distance;
  bool is_mouth_open;
};

class LandmarkOperations {
public:
  void landmark_inference(
    const cv::Mat& frame,
    const cv::Mat& faces,
    cv::Ptr<cv::face::Facemark>& facemark
  );

  DetectMouthReturn detect_mouth_open(const cv::Mat& frame);
  std::pair<bool, bool> detect_blink(const cv::Mat& frame);
  std::pair<int, int> get_upper_nose(const cv::Mat& frame);

private:
  double get_ear(const std::vector<cv::Point2i>& points);

  std::vector<std::vector<cv::Point2f>> m_landmarks;
  bool m_is_inference_sucessful;
};

#endif // LANDMARK_H_