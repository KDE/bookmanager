find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
include(KDE4Defaults)

set(structuretokenizer_SRCS
    tokenizertest.cpp
)

enable_testing()

kde4_add_unit_test(structuretokenizertest
    ${structuretokenizer_SRCS}
)

target_link_libraries(structuretokenizertest
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KDE4_KDEUI_LIBS}
)

