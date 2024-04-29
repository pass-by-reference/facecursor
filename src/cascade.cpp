#include <vector>

#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include "cascade.h"
#include "utils.h"

const static int MAX_VIDEO_WIDTH = 1920;
const static int MAX_VIDEO_HEIGHT = 1080;

const static double percentage = 0.15;
static int score = 0;
const static int MAX_TIME_SECONDS = 5;

void cascade_proto() {
  cv::VideoCapture video(0); // Get camera
  video.set(cv::CAP_PROP_FRAME_WIDTH, MAX_VIDEO_WIDTH);
  video.set(cv::CAP_PROP_FRAME_HEIGHT, MAX_VIDEO_HEIGHT);

  const std::string& output_file = 
    "cascade_" + utils::get_now_formatted() + ".mp4";
  cv::VideoWriter video_recorder(
    output_file,
    cv::VideoWriter::fourcc('m', 'p', 'e', 'g'),
    5,
    cv::Size(MAX_VIDEO_WIDTH, MAX_VIDEO_HEIGHT),
    true);

  const std::string window_name = "Live Tracking";
  cv::namedWindow(window_name, 1);

  auto start = std::chrono::steady_clock::now();

  const std::string filename =
    cv::samples::findFile("data/haarcascades/haarcascade_frontalface_alt.xml");
  cv::CascadeClassifier face_cascade(filename);
  cv::Mat pre_flip;
  cv::Mat frame;
  cv::Mat working;

  while(true) {
    video >> pre_flip;

    if(pre_flip.empty()) {
      break;
    }

    cv::flip(pre_flip, frame, 1);
    cv::cvtColor(frame, working, cv::COLOR_BGR2RGB);

    std::vector<cv::Rect> obj;
    std::vector<int> num_detections;

    face_cascade.detectMultiScale(
      working, obj, num_detections
    );

    for(const cv::Rect& rect : obj) {
      cv::rectangle(frame, rect, cv::Scalar( 255, 0, 0 ), 2, 1 );
    }

    cv::imshow(window_name, frame);
    video_recorder.write(frame);

    int key = cv::waitKey(1);
    if(key == 'q' || key == 27) {
      break;
    }
  }

  video_recorder.release();
  video.release();

}