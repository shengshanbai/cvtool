cmake_minimum_required(VERSION 3.10.0)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(myproject CXX)
project(cvtool)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(WITH_CUDA "use cuda to accelerate" OFF)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

IF (WIN32)
    set(EXPORT "__declspec(dllexport)")
ELSEIF (APPLE)
	set(EXPORT "__attribute__ ((visibility (\"default\")))")
ELSEIF (UNIX)
    set(EXPORT "__attribute__ ((visibility (\"default\")))")
ENDIF ()

configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(image_segment)
include_directories(image_segment)

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
target_link_libraries(cvtool libimage_segment ${OpenCV_LIBS})
