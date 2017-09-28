cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_CXX_FLAGS "-std=c++11")

# define, up to you

project( libBasics )

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

#head file path  

set( OpenCV_DIR /home/bruce/opencv-2.4.12/release )
set(OpenCV_FOUND 1)
find_package( OpenCV REQUIRED )

include_directories(${CMAKE_BINARY_DIR})

set(LIB_FILES
	basics.cpp
	)

#include Point Grey

link_directories(/usr/lib)

#source directory  

LINK_DIRECTORIES(${CMAKE_BINARY_DIR})

add_library( ${PROJECT_NAME} SHARED ${LIB_FILES} )
#add_executable( ${PROJECT_NAME} ${LIB_FILES})

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS})