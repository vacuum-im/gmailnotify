#Plugin file name
TARGET              = gmailnotify
include(config.inc)

#Project Configuration
TEMPLATE            = lib
CONFIG             += plugin
QT                  = core gui xml
LIBS               += -l$${TARGET_UTILS}
LIBS               += -L$${VACUUM_LIB_PATH}
DEPENDPATH         += $${VACUUM_SRC_PATH}
INCLUDEPATH        += $${VACUUM_SRC_PATH}

#Install
include(install.inc)

#Translation
include(translations.inc)

include(gmailnotify.pri)
