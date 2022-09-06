cmake_minimum_required(VERSION 2.8)
project(Capture)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(Capture Capture.cpp)
target_link_libraries(Capture ${OpenCV_LIBS} -lwiringPi -lmariadbclient -lmysqlclient tesseract)

add_executable(Capture_with_socket Capture_with_socket.cpp)
target_link_libraries(Capture_with_socket ${OpenCV_LIBS} -lwiringPi -lmariadbclient -lmysqlclient tesseract)

add_executable(Capture_Inout Capture_Inout.cpp)
target_link_libraries(Capture_Inout ${OpenCV_LIBS} -lwiringPi -lmariadbclient -lmysqlclient tesseract)
