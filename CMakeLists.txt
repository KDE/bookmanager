project(bookmanager)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )


set(bookmanager_SRCS  
    main.cpp
    shell.cpp
    collectiondb.cpp
    collection.cpp
 )

kde4_add_executable(bookmanager ${bookmanager_SRCS})

target_link_libraries(bookmanager ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})




########### install files ###############
install(TARGETS bookmanager ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES bookmanager.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES bookmanagerui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bookmanager )