cmake_minimum_required(VERSION 2.8)

project(gmailnotify-plugin)
set(PLUGIN_NAME "gmailnotify")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

list(APPEND QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml)

set(SOURCES gmailnotify.cpp notifygmaildialog.cpp)
set(HEADERS igmailnotify.h definitions.h notifygmaildialog.h gmailnotify.h)
set(MOC_HEADERS notifygmaildialog.h gmailnotify.h)
set(UIS notifygmaildialog.ui)

include_directories("${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

include("${CMAKE_SOURCE_DIR}/cmake/config.cmake")
include("${CMAKE_SOURCE_DIR}/translations/languages.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/plugin.cmake")

install(DIRECTORY "resources/menuicons"
	DESTINATION "${INSTALL_RESOURCES}")
