## Working Title

## Building

Requirements:
- git v2.40.1
- cmake v3.26.3
- clang v14.0.0

1. Run `bash setup.bash build`
- Downloads and builds dependencies (opencv)
- Download relevant models
- Setup release and debug directories
2. `cd release` && `make`
- To compile
3. `./facecursor`
- To run
4. Modify the variables in `constants.h` to fine tune experience.