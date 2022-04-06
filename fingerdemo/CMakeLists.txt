cmake_minimum_required(VERSION 3.8)
project(demo VERSION 2.0 LANGUAGES CXX C)

add_executable(fingerdemo "")

target_compile_features(fingerdemo PRIVATE cxx_std_17)

#
# List all source files and headers here
target_sources(fingerdemo
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/fingerdemo.cpp"
)


find_package(OpenCV REQUIRED)
include_directories(fingerdemo ${OpenCV_INCLUDES})
target_link_libraries(fingerdemo ${OpenCV_LIBRARIES})
#
# demo depends on gelsightsdk and gslicense
#
find_package(GelSightSdk REQUIRED)
include_directories(fingerdemo ${GelSightSdk_INCLUDE_DIRS})
target_link_libraries(fingerdemo ${GelSightSdk_LIBRARIES})

#
# Link to libpng
find_package(PNG REQUIRED)
target_link_libraries(fingerdemo ${PNG_LIBRARY})
target_link_libraries(fingerdemo uuid)

if (UNIX)

#
# C++ Filesystem 
#
# this should be found automatically depends on c++ version
#add_library(stdc++fs UNKNOWN IMPORTED)
#set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")
#
target_link_libraries(fingerdemo stdc++fs)

endif (UNIX)




