cmake_minimum_required(VERSION 3.8)
project(demo VERSION 2.0 LANGUAGES CXX C)

add_executable(demo "")

target_compile_features(demo PRIVATE cxx_std_17)

#
# List all source files and headers here
target_sources(demo
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/demo.cpp"
)


#
# demo depends on gelsightsdk and gslicense
#
find_package(GelSightSdk REQUIRED)
include_directories(demo ${GelSightSdk_INCLUDE_DIRS})
target_link_libraries(demo ${GelSightSdk_LIBRARIES})


if (UNIX)

#
# C++ Filesystem 
#
# this should be found automatically depends on c++ version
#add_library(stdc++fs UNKNOWN IMPORTED)
#set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")
#
target_link_libraries(demo stdc++fs)

endif (UNIX)


#
# Link to opencv

find_package(OpenCV REQUIRED)
target_link_libraries(demo ${OpenCV_LIBRARIES})
target_include_directories(demo PUBLIC ${OpenCV_INCLUDES})   

#set(OpenCV_DIR "$ENV{GS_THIRDPARTYDIR}/opencv/lib/cmake/opencv4/")
#if (NOT DEFINED OpenCV_DIR OR NOT IS_DIRECTORY ${OpenCV_DIR})
#   target_link_libraries(demo "$ENV{GS_THIRDPARTYDIR}/opencv/lib")
#   target_include_directories(demo PUBLIC "$ENV{GS_THIRDPARTYDIR}/opencv/include/opencv4/")

message(STATUS "Using opencv includes in ${OpenCV_INCLUDES}")



