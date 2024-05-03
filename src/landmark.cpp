#include "landmark.h"

#include "constants.h"

#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>

#include <opencv2/imgproc.hpp>

#include <iostream>

// namespace landmark {

double LandmarkOperations::get_ear(const std::vector<cv::Point2i>& points) {
  if(points.size() != 6) {
    return 1;
  }

  const auto p1 = points[0];
  const auto p2 = points[1];
  const auto p3 = points[2];
  const auto p4 = points[3];
  const auto p5 = points[4];
  const auto p6 = points[5];

  const double p2_p6 = cv::norm(p2 - p6);
  const double p3_p5 = cv::norm(p3 - p5);
  const double p1_p4 = cv::norm(p1 - p4);

  double ear = (p2_p6 + p3_p5) / (2 * p1_p4);

  return ear;
}

void LandmarkOperations::landmark_inference(
  const cv::Mat& frame,
  const cv::Mat& faces,
  cv::Ptr<cv::face::Facemark>& facemark
)
{
  m_is_inference_sucessful = false;
  if(faces.rows <= 0) {
    return;
  }

  cv::Rect face_bounded = cv::Rect2i(
    int(faces.at<float>(0, 0)),
    int(faces.at<float>(0, 1)),
    int(faces.at<float>(0, 2)),
    int(faces.at<float>(0, 3)));

  std::vector<cv::Rect> face_rect = {face_bounded};
  m_is_inference_sucessful = facemark->fit(frame, face_rect, m_landmarks);
}

DetectMouthReturn LandmarkOperations::detect_mouth_open(const cv::Mat& frame)
{
  if(!m_is_inference_sucessful) {
    return {
      .distance = 0,
      .is_mouth_open = false
    };
  }

  if(m_landmarks.size() == 0 ||
    (m_landmarks.size() >= 0 && m_landmarks[0].size() < 58)) {
    return {
      .distance = 0,
      .is_mouth_open = false
    };
  } 

  auto mouth_top = m_landmarks[0][51]; // Top lip
  auto mouth_bottom = m_landmarks[0][57]; // Bottom lip

  double distance = cv::norm(mouth_top - mouth_bottom);

  if(!constants::USE_LEFT_EYE_BLINK) {
    cv::putText(
      frame,
      "Mouth Lip Distance: " + std::to_string(distance),
      cv::Point(20,40), 
      cv::FONT_HERSHEY_SIMPLEX, 
      0.5, 
      cv::Scalar(50,170,50), 
      2
    );

    cv::circle(frame,mouth_top,3,cv::Scalar(0,255,0),cv::FILLED);
    cv::circle(frame,mouth_bottom,3,cv::Scalar(0,255,0),cv::FILLED);
  }

  return {
    .distance = static_cast<int>(distance),
    .is_mouth_open = distance > constants::MOUTH_OPEN_DISTANCE
  };
}

std::pair<bool, bool> LandmarkOperations::detect_blink(const cv::Mat& frame) {

  if(!m_is_inference_sucessful) {
    return std::make_pair(false, false);
  }

  std::vector<cv::Point2i> left;
  std::vector<cv::Point2i> right;
  for(unsigned long k=0; k < m_landmarks[0].size(); ++k) {
    // left
    if(k >= 36 && k < 42) {
      left.push_back(m_landmarks[0][k]);

      if(constants::USE_LEFT_EYE_BLINK) {
        cv::circle(frame, m_landmarks[0][k],3,cv::Scalar(0,0,255),cv::FILLED);
      }
    }
    // right
    if(k >= 42 && k < 48) {
      right.push_back(m_landmarks[0][k]);
      cv::circle(frame, m_landmarks[0][k],3,cv::Scalar(0,255,0),cv::FILLED);
    }
  }

  double right_ear = get_ear(right);
  double left_ear = get_ear(left);

  if(constants::USE_LEFT_EYE_BLINK) {
    cv::putText(
      frame,
      "Left Eye EAR: " + std::to_string(left_ear),
      cv::Point(20,40), 
      cv::FONT_HERSHEY_SIMPLEX, 
      0.5, 
      cv::Scalar(50,170,50), 
      2
    );
  }

  cv::putText(
    frame,
    "Right Eye EAR: " + std::to_string(right_ear),
    cv::Point(20,80), 
    cv::FONT_HERSHEY_SIMPLEX, 
    0.5, 
    cv::Scalar(0,170,50), 
    2
  );

  return std::make_pair(
    left_ear < constants::EAR_THRESHOLD,
    right_ear < constants::EAR_THRESHOLD  
  );
}

std::pair<int, int> LandmarkOperations::get_upper_nose(const cv::Mat& frame)
{

  if(!m_is_inference_sucessful) {
    return std::make_pair(0,0);
  }

  if(m_landmarks.size() == 0 ||
    (m_landmarks.size() >= 0 && m_landmarks[0].size() < 29)) {
    return std::make_pair(0,0);
  } 

  cv::Point2i point = m_landmarks[0][28];

  return std::make_pair(point.x, point.y);

}

// } // namespace landmark