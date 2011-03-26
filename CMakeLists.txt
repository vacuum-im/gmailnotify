cmake_minimum_required(VERSION 2.8)

project(gmailnotify-plugin)
set(PLUGIN_NAME "gmailnotify")

add_custom_target(updatets)

include("translations.cmake")
include("install.cmake")

find_package(Qt4 REQUIRED)

set(LANGS "" CACHE STRING "List of languages to build localization for (this variable overrides any of LANG_*)")
set(USED_LANGS "" CACHE INTERNAL "List of languages actually used for generating targets")
add_lang_options()

if ("${LANGS}" STREQUAL "")
	process_lang_options()
else ("${LANGS}" STREQUAL "")
	set_enabled_langs("${LANGS}")
endif ("${LANGS}" STREQUAL "")

set(QT_USE_QTXML TRUE)
#set(QT_USE_QTWEBKIT TRUE)

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif (WIN32)

include(${QT_USE_FILE})

add_definitions(-DQT_PLUGIN -DQT_SHARED)

if (WIN32)
	include_directories("${CMAKE_INSTALL_PREFIX}/sdk")
elseif (UNIX)
	include_directories("${CMAKE_INSTALL_PREFIX}/include/vacuum")
endif (WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES gmailnotify.cpp notifygmaildialog.cpp )
set(HEADERS igmailnotify.h definitions.h notifygmaildialog.h gmailnotify.h)
set(MOC_HEADERS notifygmaildialog.h gmailnotify.h)
set(UIS notifygmaildialog.ui )

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_wrap_ui(UI_HEADERS ${UIS})
#qt4_add_resources(RCC_SOURCES ${RCCS})

add_translations(TRANSLATIONS ${PLUGIN_NAME} ${HEADERS} ${SOURCES})

add_library(${PLUGIN_NAME} SHARED ${SOURCES} ${MOC_SOURCES} ${RCC_SOURCES} ${UI_HEADERS} ${TRANSLATIONS} )

find_library(TARGET_UTILS vacuumutils ${CMAKE_INSTALL_PREFIX})
target_link_libraries(${PLUGIN_NAME} ${TARGET_UTILS} ${QT_LIBRARIES})
if (WIN32)
	install(TARGETS ${PLUGIN_NAME}
		RUNTIME DESTINATION "${INSTALL_PLUGINS}"
		COMPONENT ${PLUGIN_NAME})
else (WIN32)
	install(TARGETS ${PLUGIN_NAME}
		LIBRARY DESTINATION "${INSTALL_PLUGINS}"
		COMPONENT ${PLUGIN_NAME})
endif (WIN32)

install(DIRECTORY "resources/menuicons"
	DESTINATION "${INSTALL_RESOURCES}")

