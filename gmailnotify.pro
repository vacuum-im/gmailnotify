include(config.inc)

#Project Configuration
TARGET              = gmailnotify
TEMPLATE            = lib
CONFIG             += plugin
QT                  = core gui xml
LIBS               += -L$${SDK_BIN_PATH}/libs
LIBS               += -l$${TARGET_UTILS}
DEPENDPATH         += $${SDK_SRC_PATH}
INCLUDEPATH        += $${SDK_SRC_PATH}

#Translation
TRANSLATIONS        = ./translations/src/ru_RU/$${TARGET}.ts

include(gmailnotify.pri)
