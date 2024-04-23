# Facecursor

A repo to control your mouse using your head.

Make sure you have a webcam plugged in.

## Building

Requirements:
- git v2.40.1
- cmake v3.26.3
- clang v14.0.0

1. Run `bash setup.bash build`
- Downloads and builds dependencies (opencv)
- Download relevant models
- Setup release and debug directories
2. To Compile: `cd release` && `make`
3. To Run: `./facecursor`
4. Modify the variables in `constants.h` to fine tune experience.