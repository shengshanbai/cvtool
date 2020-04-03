cmake_minimum_required(VERSION 3.10.0)

project(cvtool)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(WITH_CUDA "use cuda to accelerate" OFF)

set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE 
main.cpp
show_video.h
show_video.cpp
)

if(WITH_CUDA)
    find_package(CUDA REQUIRED)
    add_definitions(-DWITH_CUDA)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-extended-lambda -gencode arch=compute_75,code=sm_75")
    set(SOURCE ${SOURCE} 
    gpuUtil.h 
    gpuUtil.cpp)
endif(WITH_CUDA)

ADD_EXECUTABLE(cvtool ${SOURCE})
target_link_libraries(cvtool ${OpenCV_LIBS})
