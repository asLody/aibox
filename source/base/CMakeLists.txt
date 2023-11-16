add_library(base STATIC
        types.h
        logging.cpp
        logging.h)

target_link_libraries(base fmt::fmt)