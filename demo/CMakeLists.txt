cmake_minimum_required(VERSION 3.2)
project(demo VERSION 1.0 LANGUAGES CXX C)

add_executable(demo "")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

#
# List all source files and headers here
target_sources(demo
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/demo.cpp"
)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#set(CMAKE_SHARE_LINKER_FLAGS "${CMAKE_SHARE_LINKER_FLAGS} -fopenmp")

#
# demo depends on gelsightsdk and gslicense
#
find_package(GelSightSdk REQUIRED)
include_directories(demo ${GelSightSdk_INCLUDE_DIRS})
target_link_libraries(demo ${GelSightSdk_LIBRARIES})


#
# pthread
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#target_link_libraries(gscoretests Threads::Threads)



#
# C++ Filesystem 
add_library(stdc++fs UNKNOWN IMPORTED)
set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")
target_link_libraries(demo stdc++fs)




#set_property(TARGET demo PROPERTY CXX_STANDARD 14)


