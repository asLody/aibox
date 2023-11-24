add_library(base STATIC
        types.h
        logging.cpp
        logging.h
        macros.h
        file_utils.cpp
        file_utils.h)

target_link_libraries(base fmt::fmt -lpthread)