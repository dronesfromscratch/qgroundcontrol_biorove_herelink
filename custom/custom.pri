message("Adding Custom Herelink/Biorove Plugin")

#-- Version control
#   Major and minor versions are defined here (manually)

CUSTOM_QGC_VER_MAJOR = 4
CUSTOM_QGC_VER_MINOR = 4
CUSTOM_QGC_VER_PATCH = 5
CUSTOM_QGC_VER_FIRST_BUILD = 0

linux {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-strict-aliasing
}

# Build number is automatic
# Uses the current branch. This way it works on any branch including build-server's PR branches
CUSTOM_QGC_VER_BUILD = $$system(git --git-dir ../.git rev-list $$GIT_BRANCH --first-parent --count)
win32 {
    CUSTOM_QGC_VER_BUILD = $$system("set /a $$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD")
} else {
    CUSTOM_QGC_VER_BUILD = $$system("echo $(($$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD))")
}
CUSTOM_QGC_VERSION = $${CUSTOM_QGC_VER_MAJOR}.$${CUSTOM_QGC_VER_MINOR}.$${CUSTOM_QGC_VER_PATCH}.$${CUSTOM_QGC_VER_BUILD}

DEFINES -= APP_VERSION_STR=\"\\\"$$APP_VERSION_STR\\\"\"
DEFINES += APP_VERSION_STR=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

message(Custom QGC Version: $${CUSTOM_QGC_VERSION})

DEFINES += CUSTOMHEADER=\"\\\"HerelinkCorePlugin.h\\\"\"
DEFINES += CUSTOMCLASS=HerelinkCorePlugin

# Remove code which the Herelink doesn't need
DEFINES += \
    QGC_GST_TAISYNC_DISABLED
    NO_SERIAL_LINK
    QGC_DISABLE_BLUETOOTH

# Enable Herelink AirUnit video config
DEFINES += \
    QGC_HERELINK_AIRUNIT_VIDEO

# CONFIG += AndroidHomeApp

# BIOROVE BRANDING -------------------------------------------------------------------------------------------------


TARGET   = BioControl-Herelink
DEFINES += QGC_APPLICATION_NAME='"\\\"Biorove GCS\\""'

DEFINES += QGC_ORG_NAME=\"\\\"qgroundcontrol.org\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"org.qgroundcontrol\\\"\"

QGC_APP_NAME        = "BioControl"
QGC_BINARY_NAME     = "BioControl"
QGC_ORG_NAME        = "Biorove"
QGC_ORG_DOMAIN      = "org.biorove"
QGC_ANDROID_PACKAGE = "org.custom.qgroundcontrol"
QGC_APP_DESCRIPTION = "BioControl"
QGC_APP_COPYRIGHT   = "Copyright (C) 2025 Biorove. All rights reserved."

#END BIOROVE Branding ------------------------------------------------------------------------------------------------------


# Our own, custom resources
message(Adding custom resources $$PWD/custom.qrc)
RESOURCES += \
    $$PWD/custom.qrc

QML_IMPORT_PATH += \
   $$PWD/src

# Herelink and biorove specific custom sources
SOURCES += \
    $$PWD/src/HerelinkCorePlugin.cc \
    $$PWD/src/HerelinkOptions.cc \    
    $$PWD/src/customgpswaypointrecoder.cpp

HEADERS += \
    $$PWD/src/HerelinkCorePlugin.h \
    $$PWD/src/HerelinkOptions.h \
    $$PWD/src/customgpswaypointrecoder.h

CUSTOM_QGC_VERSION = $${CUSTOM_QGC_VER_MAJOR}.$${CUSTOM_QGC_VER_MINOR}.$${CUSTOM_QGC_VER_BUILD}

DEFINES -= GIT_VERSION=\"\\\"$$GIT_VERSION\\\"\"
DEFINES += GIT_VERSION=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"


INCLUDEPATH += \
    $$PWD/src \

message(Custom QGC Version: $${CUSTOM_QGC_VERSION})

#-------------------------------------------------------------------------------------
# Custom Firmware/AutoPilot Plugin

INCLUDEPATH += \    
    $$PWD/src/FirmwarePlugin \
    $$PWD/herelink \
    $$PWD/src/AutoPilotPlugin

message("Including custom Herelink sources for source switching")

SOURCES += $$PWD/herelink/VideoStreamControl.cc
HEADERS += $$PWD/herelink/VideoStreamControl.h

