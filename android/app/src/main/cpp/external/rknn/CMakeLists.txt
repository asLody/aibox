add_library(rknn SHARED IMPORTED GLOBAL)
set_target_properties(rknn PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/librknnrt.so)
target_include_directories(rknn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)