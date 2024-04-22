#!/bin/bash
# ============================================
#
# Generic Script To Aid In Building
#
# Primarly tested with linux
# 
# Generic Steps
# 1. Building opencv from source
# 2. Downloading relevant models
# 3. Setup build files
# ============================================

OPENCV_VERSION="4.9.0"
OPENCV_GITHUB_URL="https://github.com/opencv/opencv.git"
OPENCV_GITHUB_CONTRIB_URL="https://github.com/opencv/opencv_contrib.git"

OPENCV_BUILD_DIR="opencv_release_${OPENCV_VERSION}"

RELEASE_DIR="release"
DEBUG_DIR="debug"

FACIAL_LANDMARK_DOWNLOAD_URL="https://raw.githubusercontent.com/kurnianggoro/GSOC2017/master/data/lbfmodel.yaml"
YUNET_MODEL_URL="https://github.com/opencv/opencv_zoo/raw/4.9.0/models/face_detection_yunet/face_detection_yunet_2023mar.onnx"

ensure_clang() {
  if command -v clang
  then
    VERSION=$(clang --version)
    echo "Using" $VERSION
    return 0
  else
    echo "clang cannot be found. Probably not installed"
    return 1
  fi
}

ensure_git() {
  if command -v git
  then
    VERSION=$(git --version)
    echo "Using" $VERSION
    return 0
  else
    echo "Git command cannot be found. Probably not installed"
    return 1
  fi
}

ensure_cmake() {
  if command -v cmake
  then
    VERSION=$(cmake --version)
    echo "Using" $VERSION
    return 0
  else
    echo "Cmake command cannot be found. Probably not installed"
    return 1
  fi
}

install_and_build_dependencies() {
  if ensure_git && ensure_cmake && ensure_clang;
  then
    echo "Cmake, clang, git found"
  else
    echo "Either git, cmake, or clang were not found or installed"
    return 1
  fi

  # Clone and setup opencv build directory
  git clone ${OPENCV_GITHUB_URL}
  git clone ${OPENCV_GITHUB_CONTRIB_URL}  

  # Get specific version.
  cd opencv && git checkout ${OPENCV_VERSION} && cd ..
  cv opencv_contrib && git checkout ${OPENCV_VERSION} && cd ..

  mkdir ${OPENCV_BUILD_DIR}
  cd ${OPENCV_BUILD_DIR}

  # Setup
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -D BUILD_EXAMPLES=ON \
    -D WITH_GTK=ON \
    -D WITH_OPENGL=ON \
    -D WITH_FFMPEG=ON \
    -D OPENCV_EXTRA_MODULES_PATH="../opencv_contrib/modules/face" \
    ../opencv

  # Compile from source
  cmake --build . -- -j8
  
  cd ..
}

setup_build() {
  # Build
  if [ ! -d "$RELEASE_DIR" ]; then
    mkdir "$RELEASE_DIR" && cd "$RELEASE_DIR"
    cmake -D CMAKE_BUILD_TYPE=Release ..
    cd ..
  fi

  # Debug
  if [ ! -d "$DEBUG_DIR" ]; then
    mkdir "$DEBUG_DIR" && cd "$DEBUG_DIR"
    cmake -D CMAKE_BUILD_TYPE=Debug ..
    cd ..
  fi
}

download_models() {
  if [ -d $RELEASE_DIR ]; then
    cd $RELEASE_DIR
    curl $FACIAL_LANDMARK_DOWNLOAD_URL > lbfmodel.yaml
    curl $YUNET_MODEL_URL > face_detection_yunet_2023mar.onnx
    cd ..
  else
    echo "Release directory not found"
  fi

  if [ -d $DEBUG_DIR ]; then
    cd $DEBUG_DIR
    curl $FACIAL_LANDMARK_DOWNLOAD_URL > lbfmodel.yaml
    curl $YUNET_MODEL_URL > face_detection_yunet_2023mar.onnx
    cd ..
  else
    echo "Debug directory not found"
  fi
}

build() {
  install_and_build_dependencies
  setup_build
  download_models
}

clean() {

  # Remove opencv repo
  if [ -d "opencv" ]; then
    rm -rf opencv
  fi

  # Remove extra modules repo
  if [ -d "opencv_contrib" ]; then
    rm -rf opencv_contrib
  fi

  if [ -d $OPENCV_BUILD_DIR ]; then
    rm -r $OPENCV_BUILD_DIR
  fi

  if [ -d $RELEASE_DIR ]; then
    rm -r $RELEASE_DIR
  fi

  if [ -d $DEBUG_DIR ]; then
    rm -r $DEBUG_DIR
  fi
}

ensure_clang

if [[ $# == 1 && $1 == "clean" ]]; then
  clean
fi

if [[ $# == 1 && $1 == "build" ]]; then
  build
fi

if [[ $# == 1 && $1 == "download_models" ]]; then
  download_models
fi

if [[ $# == 0 ]]; then
  echo "No argument provided. Either run with build, clean, or download_models"
fi