# sdkdemo

This project is a basic example of how to use GelSightSDK. The SDK provides
functions and data structures for converting images from GelSight systems into
3D heightmaps. 

## Windows

This project assumes you have the `GS_SDK` environment variable set to the path
to your GelSightSdk folder. You also need a valid license for GelSightSdk.

 * Run the copysdk.bat script to copy the dlls from your GelSightSdk folder into the build directories
 * Open the solution and build

## Linux

This project assumes you have the `GS_SDK` environment variable set to the path
to your GelSightSdk folder. You also need a valid license for GelSightSdk.

Install the license
 * sudo mkdir 777 /etc/gelsight
 * sudo cp <license file> /etc/gelsight/gelsight_64.bin

To build and run on linux, you need the following pkgs
 * sudo apt-get update
 * sudo apt-get install cmake
 * sudo apt-get install libpng-dev
 * sudo apt install libeigen3-dev
 * sudo apt install libeigen3-dev
 * sudo apt-get install libgtest-dev
 * sudo apt-get install libfl-dev
 * sudo apt install subversion
 * sudo apt-get install uuid-dev
 * sudo apt-get libopencv-dev python3-dev

Set the environment variables
 * export GS_SDK=path_to_sdk
 * export LD_LIBRARY_PATH=path_to_sdk/lib

Use CMake to create the make files
 * mkdir build
 * cd build
 * cmake ..; make

Run the demo
 * cd build
 * ./demo/demo

