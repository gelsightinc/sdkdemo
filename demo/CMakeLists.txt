cmake_minimum_required(VERSION 3.2)
project(demo VERSION 1.0 LANGUAGES CXX C)

add_executable(demo "")

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




