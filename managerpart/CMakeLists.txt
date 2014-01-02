add_subdirectory(utils)
add_subdirectory(tools)

include_directories(${POPPLER_QT4_INCLUDE_DIR})
include_directories(utils)

set(bookmanager_manager_PART_SRCS
    searchpage.cpp
    bookmanagerpart.cpp
    importdialog.cpp
    modifydialog.cpp
    queryengine.cpp
    collectiondb.cpp
    bookstruct.h
    collectionmodel.cpp
    collectiontreemodel.cpp
    bookdelegate.cpp
    bookiconbuilder.cpp
    booktreeview.cpp
    bookdetailswidget.cpp
    backupcollectionassistant.cpp
    collectionorganizerwidget.cpp
)
qt4_add_dbus_adaptor(bookmanager_manager_PART_SRCS org.bookmanager.BookManagerPart.xml
                    bookmanagerpart.h BookManagerPart)

kde4_add_ui_files(bookmanager_manager_PART_SRCS
    importwidget.ui
    uis/introductionpage.ui
    uis/collectionorganizerwidget.ui
)

kde4_add_plugin(bookmanagerpart ${bookmanager_manager_PART_SRCS})

target_link_libraries(bookmanagerpart
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_THREADWEAVER_LIBRARIES}
    ${QT_QTSQL_LIBRARIES}
    ${POPPLER_QT4_LIBRARIES}
    csvLibrary
)

install(TARGETS bookmanagerpart DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES bookmanagerpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES managerpart.rc DESTINATION ${DATA_INSTALL_DIR}/bookmanager)
