 cmake_minimum_required(VERSION 3.8)
project(gsdemo VERSION 2.0 LANGUAGES CXX)

#
# cmake module path for custom modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

add_subdirectory(demo)
add_subdirectory(fingerdemo)

