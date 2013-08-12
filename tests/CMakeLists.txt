set( collectiondbtests_SRCS 
    collectiondbtests.cpp
    ../managerpart/collectiondb.cpp
)

kde4_add_unit_test( collectiondbtests
    TESTNAME managerpart-collectiondb
    ${collectiondbtests_SRCS}
)

target_link_libraries( collectiondbtests
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTSQL_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
)