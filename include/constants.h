#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace constants {

const bool SIMULATE_CLICK = true;
const bool SIMULATE_CURSOR_MOVE = true;
const bool DEBUG = true;
const bool RECORD = true;

const bool USE_LEFT_EYE_BLINK = false;

const double RESTING_MOUTH_DISTANCE = 25;
const double MOUTH_OPEN_DISTANCE = 50;
const double EAR_THRESHOLD = 0.20;

const int BLINK_THRESHOLD_MILLISECONDS = 400;

// How long (in frames) a face gesture (mouth open or blink)
// must be held for until a mouse click is activated.
const int RIGHT_CLICK_FRAMES_THRESHOLD = 10;
const int LEFT_CLICK_FRAMES_THRESHOLD = 5;

} // namespace constants

#endif // CONSTANTS_H_