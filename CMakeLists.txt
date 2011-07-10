project(bookmanager)
#add the kpart directory

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_subdirectory(managerpart)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})


set(bookmanager_SRCS   
    main.cpp
    shell.cpp
    readerpage.cpp
 )

kde4_add_kcfg_files(bookmanager_SRCS bookmanagerconfig.kcfgc)
kde4_add_executable(bookmanager ${bookmanager_SRCS})

target_link_libraries(bookmanager ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KNEWSTUFF3_LIBS} ${QT_QTSQL_LIBRARIES})


########### add cpack support to creat packages ##########
###from the cmake wiki

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple Ebook Collection manager")
SET(CPACK_PACKAGE_VENDOR "")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_STRIP_FILES "bin/MyExecutable")
SET(CPACK_SOURCE_STRIP_FILES "")
SET(CPACK_PACKAGE_EXECUTABLES "bookmanager" "Book Manager")
###for rpms
SET(CPACK_RPM_PACKAGE_RELEASE "1")
SET(CPACK_RPM_PACKAGE_LICENSE "GPL Version 2")
INCLUDE(CPack)

########### install files ###############
install(TARGETS bookmanager ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES bookmanager.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES bookmanagerui.rc  DESTINATION  ${DATA_INSTALL_DIR}/bookmanager )
install( FILES bookmanagerconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR})
