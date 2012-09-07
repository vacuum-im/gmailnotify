cmake_minimum_required(VERSION 2.8)

project(gmailnotify-plugin)
set(PLUGIN_NAME "gmailnotify")

set(SOURCES gmailnotify.cpp notifygmaildialog.cpp)
set(HEADERS igmailnotify.h definitions.h notifygmaildialog.h gmailnotify.h)
set(MOC_HEADERS notifygmaildialog.h gmailnotify.h)
set(UIS notifygmaildialog.ui)

include_directories("${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

include("${CMAKE_SOURCE_DIR}/cmake/config.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/translations.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/plugin.cmake")

install(DIRECTORY "resources/menuicons"
	DESTINATION "${INSTALL_RESOURCES}")
