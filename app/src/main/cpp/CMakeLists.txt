cmake_minimum_required(VERSION 3.22.1)
project("aibox")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(core)

add_library(aibox SHARED jni.cpp)
target_link_libraries(aibox core)

target_link_libraries(aibox android log)