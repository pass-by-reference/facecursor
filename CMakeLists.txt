cmake_minimum_required(VERSION 3.10)
project(facecursor VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv_release_4.9.0) # Can change depending on version

if(UNIX AND NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-lX11 -lXtst")
endif()

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-framework ApplicationServices")
endif()

include_directories(include)

set(FACECURSOR_SOURCES
  main.cpp
  src/pointer_webcam.cpp
  src/cursor_mac.cpp
  src/cursor_linux.cpp
  src/landmark.cpp
  src/face_to_cursor.cpp
  src/click_processor.cpp
)

add_executable(facecursor ${FACECURSOR_SOURCES})

find_package(OpenCV REQUIRED)
target_link_libraries(facecursor PUBLIC ${OpenCV_LIBS})
target_include_directories(facecursor PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )