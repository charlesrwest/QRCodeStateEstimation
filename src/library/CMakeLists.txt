cmake_minimum_required (VERSION 2.8.3)

file(GLOB libraryHeaders *.h *.hpp)
file(GLOB librarySource *.cpp *.c)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(QRCodeStateEstimation SHARED  ${librarySource} ${libraryHeaders})
target_link_libraries(QRCodeStateEstimation zbar opencv_core opencv_highgui opencv_calib3d)

