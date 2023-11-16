cmake_minimum_required(VERSION 3.15.0)

project("AiBox")

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(app)