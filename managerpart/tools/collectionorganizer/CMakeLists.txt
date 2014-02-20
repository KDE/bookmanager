set(collectionorganizer_SRCS
    collectionorganizer.h
    collectionorganizer.cpp
    structuretokenizer.h
    structuretokenizer.cpp
    copycollectionworker.h
    copycollectionworker.cpp
    token.h
    token.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/managerpart)

kde4_add_library(collectionorganizer ${collectionorganizer_SRCS})
