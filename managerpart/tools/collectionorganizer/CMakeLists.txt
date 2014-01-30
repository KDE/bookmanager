set(collectionorganizer_SRCS
    collectionorganizer.h
    collectionorganizer.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/managerpart)

add_library(collectionorganizer ${collectionorganizer_SRCS})
