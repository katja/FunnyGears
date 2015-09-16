TARGET = "Funny Gears"      # Default: Name of .pro-file (this file)

QT *= core \                # Should be included by default
    gui \                   # Should be included by default
    widgets                 # Without this line Qt won't recognise any QtWidgets!

TEMPLATE = app              # Is anyway default, but can't harm

# Specifies project configuration and compiler options
# Possible values include:  debug, release, debug_and_release,
#                           ordered (=> SUBDIRS listed are processed in given order)
#                           warn_on, warn_off (=> print all/fewest possible compiler warnings)
CONFIG *= debug             # Only during development necessary: Print debugging symbols and information

# Add C++11 support
CONFIG += c++11

DESTDIR = "app/"                # Project will be build in this directory
OBJECTS_DIR = "compile/"        # Directory for intermediate objects when compiling
MOC_DIR = $$OBJECTS_DIR/.moc
RCC_DIR = $$OBJECTS_DIR/.rrc
UI_DIR  = $$OBJECTS_DIR/.ui

INCLUDEPATH += ./src/
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/opt/glm/include #for glm library

macx: ICON = images/Gear.icns      # Mac solution for the icon of the app

RESOURCES = images.qrc

PRECOMPILED_HEADER = src/stable.h

HEADERS = \
    src/*.h \
    src/basic_objects/*.h \
    src/graphics_objects/*.h \
    src/graphics_widgets/*.h \
    src/helper_objects/*.h \
    src/main_gui_objects/*.h \
    src/tool_bars/*.h

SOURCES = \
    src/*.cpp \
    src/basic_objects/*.cpp \
    src/graphics_objects/*.cpp \
    src/graphics_widgets/*.cpp \
    src/helper_objects/*.cpp \
    src/main_gui_objects/*.cpp \
    src/tool_bars/*.cpp
