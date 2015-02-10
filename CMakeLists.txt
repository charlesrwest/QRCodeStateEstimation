cmake_minimum_required (VERSION 2.8.3)
PROJECT(test)

#Get c++11
ADD_DEFINITIONS(-std=c++11)

#Tell cmake were to find the sub-projects
add_subdirectory(./src)
