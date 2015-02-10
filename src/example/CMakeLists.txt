cmake_minimum_required (VERSION 2.8.3)
PROJECT(test)

#Get c++11
ADD_DEFINITIONS(-std=c++11)

FILE(GLOB SOURCEFILES *.cpp *.c)

message( ${SOURCEFILES} )

#set path to library
link_directories(/usr/lib/x86_64-linux-gnu ../library/)

#Put the binary in the right location
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#Add the compilation target
ADD_EXECUTABLE(estimateLocationFromQRCode ${SOURCEFILES})

#link libraries to executable
target_link_libraries(estimateLocationFromQRCode  QRCodeStateEstimation)
