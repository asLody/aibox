add_executable(aibox main.cpp)

if (ANDROID)
    target_link_libraries(aibox android log)
endif()

target_link_libraries(aibox core)
