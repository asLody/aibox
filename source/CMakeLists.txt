cmake_minimum_required(VERSION 3.22.1)

project("AiBox")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(app)