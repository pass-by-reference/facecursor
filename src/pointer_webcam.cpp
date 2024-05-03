#include "pointer_webcam.h"
#include "constants.h"

#include "cursor_mac.h"
#include "cursor_linux.h"
#include "landmark.h"
#include "face_to_cursor.h"
#include "click_processor.h"
#include "utils.h"

#include <string>
#include <iostream>
#include <chrono>

#include <opencv2/dnn.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include <chrono>

FaceToCursor face_to_cursor;
ClickProcessor click_processor;
LandmarkOperations landmark_operations;

const int CAMERA_WIDTH = 1024;
const int CAMERA_HEIGHT = 720;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 720;

cv::Mat infer_face(cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN>& detector)
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

#ifdef __linux__
void input_operations(Display* display, Window window, const InputOperationsParams& params)
#elif __APPLE__
void input_operations(const InputOperationsParams& params)
#endif
{

  auto [x, y] = face_to_cursor.translate(params.face_position_coords);
  // auto [x, y] = process_pointer_coords(pointer_coords(params.coords));
  // std::cout << "[" << x << ", " << y << "]" << std::endl;

#ifdef __linux__
  click_processor.handle_left_click(display, params);
  click_processor.handle_right_click(display, params);
  cursor::move_mouse(display, window, x, y);
#elif __APPLE__
  click_processor.handle_left_click(face_to_cursor, params);
  click_processor.handle_right_click(face_to_cursor, params);
  cursor::move_mouse(x,y);
#endif

}

int pointer_webcam()
{
#ifdef __linux__
  Display *display;
  Window window;

  if( (display = XOpenDisplay(NULL)) == NULL ) {
    std::cout << "Cannot find display" << std::endl;
    return -1;
  }


  window = DefaultRootWindow(display);
  XAllowEvents(display, AsyncBoth, CurrentTime);

  SCREEN_WIDTH = XDisplayWidth(display, 0);
  SCREEN_HEIGHT = XDisplayHeight(display, 0);
#endif

  face_to_cursor = FaceToCursor(
    CAMERA_WIDTH,
    CAMERA_HEIGHT,
    SCREEN_WIDTH,
    SCREEN_HEIGHT
  );

  std::cout << "(" << SCREEN_WIDTH << ", " << SCREEN_HEIGHT << ")" << std::endl;

  const std::string fd_modelPath = "./face_detection_yunet_2023mar.onnx";
  float scoreThreshold = 0.8;
  float nmsThreshold = 0.3;
  int topK = 5000;

  // Initialize FaceDetectorYN
  cv::Ptr<cv::FaceDetectorYN> detector =
    cv::FaceDetectorYN::create(fd_modelPath, "", cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), scoreThreshold, nmsThreshold, topK);
  detector->setInputSize(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT));

  // Initialize facemark
  const std::string landmark_model_path = "./lbfmodel.yaml";
  cv::Ptr<cv::face::Facemark> facemark = cv::face::FacemarkLBF::create();
  facemark->loadModel(landmark_model_path);

  #ifdef __linux__
  cv::VideoCapture video(0, cv::CAP_V4L2); // Get camera
  #elif __APPLE__
  cv::VideoCapture video(0); // Get camera
  #endif

  video.set(cv::CAP_PROP_FRAME_COUNT , 30);
  video.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
  video.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
  video.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('m', 'p', '4', 'v'));

  // record video
  const std::string& output_file = 
    utils::get_now_formatted() + ".mp4";
  cv::VideoWriter video_recorder(
    output_file,
    cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    15,
    cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT),
    true);
  
  std::string window_name;
  window_name = "Live Tracking";
  cv::namedWindow(window_name, 1);

  cv::Mat frame;
  cv::Mat faces;

  video >> frame;
  if(frame.empty()) return -1;
  cv::resize(frame, frame, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT));
  
  while(true) {
    video >> frame;

    if(frame.empty()) break;
    
    cv::resize(frame, frame, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT));
    cv::flip(frame, frame, 1);

    cv::Mat faces = infer_face(frame, detector);

    landmark_operations.landmark_inference(frame, faces, facemark);

    const auto [is_left_eye_closed, is_right_eye_closed] =
      landmark_operations.detect_blink(frame);

    const DetectMouthReturn ret =
      landmark_operations.detect_mouth_open(frame);

    const bool is_mouth_open = ret.is_mouth_open;
    face_to_cursor.set_mouth_open_distance(ret.distance);

    const std::pair<int,int> face_position_coords =
      landmark_operations.get_upper_nose(frame);

    InputOperationsParams input = {
      .face_position_coords = face_position_coords,
      .is_left_eye_closed = is_left_eye_closed,
      .is_right_eye_closed = is_right_eye_closed,
      .is_mouth_open = is_mouth_open
    };

#ifdef __linux__
    input_operations(display, window, input);
#elif __APPLE__
    input_operations(input);
#endif

    if(constants::DEBUG) {
      cv::imshow(window_name, frame);
    }

    if(constants::RECORD) {
      video_recorder.write(frame);
    }

    int key = cv::waitKey(1);
    if(key == 'q' || key == 27) {
      break;
    }
  }

  video.release();
  video_recorder.release();

  return 0;
}