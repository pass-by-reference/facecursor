#include <vector>

#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include "extras/cascade_yu_test.h"
#include "utils.h"
#include "constants.h"

const static int MAX_VIDEO_WIDTH = 1920;
const static int MAX_VIDEO_HEIGHT = 1080;

const static double percentage = 0.15;
static int score = 0;
const static int MAX_TIME_SECONDS = 5;

const int CAMERA_WIDTH = 1920;
const int CAMERA_HEIGHT = 1080;

int total_frame = 0;

std::string record();
void cascade_proto(const std::string& filename);
void yunet(const std::string& filename);

void caller() {
  std::string filename = record();

  cascade_proto(filename);
  yunet(filename);
}

std::string record() {
  cv::VideoCapture video(0); // Get camera
  video.set(cv::CAP_PROP_FRAME_WIDTH, MAX_VIDEO_WIDTH);
  video.set(cv::CAP_PROP_FRAME_HEIGHT, MAX_VIDEO_HEIGHT);

  const std::string& output_file = 
    "recording.mp4";
  cv::VideoWriter video_recorder(
    output_file,
    cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    30,
    cv::Size(MAX_VIDEO_WIDTH, MAX_VIDEO_HEIGHT),
    true);

  const std::string window_name = "Tracking";
  cv::namedWindow(window_name, 1);

  cv::Mat frame;

  int index = 0;
  while(true) {

    video >> frame;

    if(frame.empty()) {
      break;
    }

    cv::flip(frame, frame, 1);
    // cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    cv::imshow(window_name, frame);
    video_recorder.write(frame);

    ++index;

    int key = cv::waitKey(1);
    if(key == 'q' || key == 27) {
      break;
    }
  }

  total_frame = index;

  video.release();
  video_recorder.release();

  return output_file;
}

void cascade_proto(const std::string& filename) {

  cv::VideoCapture video(filename); // Get recording

  const std::string output_filename =
    cv::samples::findFile("data/haarcascades/haarcascade_frontalface_alt.xml");
  cv::CascadeClassifier face_cascade(output_filename);

  const std::string& output_file = 
    "cascade_" + utils::get_now_formatted() + ".mp4";
  cv::VideoWriter video_recorder(
    output_file,
    cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    30,
    cv::Size(MAX_VIDEO_WIDTH, MAX_VIDEO_HEIGHT),
    true);

  cv::Mat frame;

  int index = 0;

  while(true) {
    video >> frame;

    if(frame.empty()) {
      break;
    }

    cv::flip(frame, frame, 1);
    // cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    std::vector<cv::Rect> obj;
    std::vector<int> num_detections;

    face_cascade.detectMultiScale(
      frame, obj, num_detections
    );

    for(const cv::Rect& rect : obj) {
      cv::rectangle(frame, rect, cv::Scalar( 255, 0, 0 ), 2, 1 );
    }

    // cv::imshow(window_name, frame);
    video_recorder.write(frame);

    std::cout << "\r" << "Processing Cascade Frame: " << index << std::flush;
    ++index;

    int key = cv::waitKey(1);
    if(key == 'q' || key == 27) {
      break;
    }
  }

  video.release();
  video_recorder.release();

}

cv::Mat infer_face_(cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN>& detector)
{
  cv::Mat faces;
  detector->detect(frame, faces);

  // Get largest face
  int face_row = 0;
  int max_width = 0;
  int max_height = 0;
  for(int i = 0; i < faces.rows; ++i) {
    int width = int(faces.at<float>(i, 2));
    int height = int(faces.at<float>(i, 3));

    if(width > max_width && height > max_height) {
      max_width = width;
      max_height = height;
      face_row = i;
    }

    if(constants::DEBUG) {
      int thickness = 2;
      cv::rectangle(
        frame, 
        cv::Rect2i(
          int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)),
          int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3))), 
        cv::Scalar(0, 255, 0),
        thickness
      );
    }
  }
  
  if(faces.rows > 0) {
    return faces.row(face_row);
  }

  return cv::Mat();
}

void yunet(const std::string& filename)
{
  cv::VideoCapture video(filename); // Get recording

  const std::string fd_modelPath = "./face_detection_yunet_2023mar.onnx";
  float scoreThreshold = 0.8;
  float nmsThreshold = 0.3;
  int topK = 5000;

  // Initialize FaceDetectorYN
  cv::Ptr<cv::FaceDetectorYN> detector =
    cv::FaceDetectorYN::create(fd_modelPath, "", cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), scoreThreshold, nmsThreshold, topK);
    detector->setInputSize(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT));

  const std::string& output_file = 
    "yunet_" + utils::get_now_formatted() + ".mp4";
  cv::VideoWriter video_recorder(
    output_file,
    cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    30,
    cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT),
    true);

  cv::Mat frame;

  int index = 0;
  while(true) {
    video >> frame;

    if(frame.empty()) break;

    cv::resize(frame, frame, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT));
    cv::flip(frame, frame, 1);

    cv::Mat faces = infer_face_(frame, detector);

    video_recorder.write(frame);

    std::cout << "\r" << "Processing Yunet Frame: " << index << std::flush;
    ++index;

    int key = cv::waitKey(1);
    if(key == 'q' || key == 27) {
      break;
    }
  }

  video.release();
  video_recorder.release();
}