add_executable(tests
        core/hid-parser.cpp
        external/librga.cpp
        core/input_device.cpp)
target_link_libraries(tests PRIVATE core)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)