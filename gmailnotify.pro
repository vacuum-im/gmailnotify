include(qmake/debug.inc)
include(qmake/config.inc)

#Project configuration
TARGET              = gmailnotify
QT                  = core gui xml

include(gmailnotify.pri)

#Default progect configuration
include(qmake/plugin.inc)

#Translation
TRANS_SOURCE_ROOT   = .
include(translations/languages.inc)
