#Path to biulded Vacuum src direcroty
isEmpty(SDK_SRC_PATH) {
  SDK_SRC_PATH           = ../vacuum/src
}
isEmpty(SDK_BIN_PATH) {
  SDK_BIN_PATH           = ../vacuum/src
}

#Debug Features
CONFIG(debug, debug|release) 
{
  #Path to copy plugin on build
  win32:DLLDESTDIR       = ../vacuum/plugins
  unix:DESTDIR           = ../vacuum/plugins

  #Path from vacumm to plugin resources
  DEFINES               += RESOURCES_DIR='\\\"../gmailnotify/resources\\\"'
}

#Should be included from SDK
TARGET_UTILS             = vacuumutils
unix:VERSION_UTILS       = 1.7.0

