set(collectionorganizer_SRCS
    collectionorganizer.h
    collectionorganizer.cpp
    structuretokenizer.h
    structuretokenizer.cpp
    token.h
    token.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/managerpart)

add_library(collectionorganizer ${collectionorganizer_SRCS})
