#based off the kontact plugin tutorial on techbase
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(bookmanager_manager_PART_SRCS 
    bookmanagerpart.cpp
    importdialog.cpp
    collection.cpp
    collectiondb.cpp
    bookstruct.h
    collectionmodel.cpp 
)
qt4_add_dbus_adaptor(bookmanager_manager_PART_SRCS org.bookmanager.BookManagerPart.xml
                    bookmanagerpart.h BookManagerPart)

kde4_add_ui_files(bookmanager_manager_PART_SRCS importdialog.ui)

kde4_add_plugin(bookmanagerpart ${bookmanager_manager_PART_SRCS})



target_link_libraries(bookmanagerpart ${KDE4_KDEUI_LIBS}  ${KDE4_KPARTS_LIBS} ${QT_QTSQL_LIBRARIES})


install( TARGETS bookmanagerpart DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES bookmanagerpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES managerpart.rc DESTINATION ${DATA_INSTALL_DIR}/bookmanager )