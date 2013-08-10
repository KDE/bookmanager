set( collectiondbtests_SRCS collectiondbtests.cpp )

kde4_add_unit_test( collectiondbtests
    TESTNAME managerpart-collectiondb
    ${collectiondbtests_SRCS}
)

target_link_libraries( collectiondbtests
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
)