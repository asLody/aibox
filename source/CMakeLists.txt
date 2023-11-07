cmake_minimum_required(VERSION 3.22.1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(core)

if (ANDROID)
    add_library(aibox SHARED jni.cpp)
    target_link_libraries(aibox android log)
else ()
    add_executable(aibox main.cpp)
endif()

target_link_libraries(aibox core)