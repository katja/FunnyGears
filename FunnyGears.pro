QT  +=  core gui \
        widgets \               # Without this line Qt won't recognise any QtWidgets!
        opengl widgets          # specify all modules used in the project here!

# Specifies project configuration and compiler options
# Possible values include:  debug, release, dubug_and_release,
#                           ordered (=> SUBDIRS listed are processed in given order)
#                           warn_on, warn_off (=> print all/fewest possible compiler warnings)
CONFIG  += debug                # Only during development necessary: Print debugging symbols and information

# Add C++11 support
CONFIG  += c++11
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7

DESTDIR = "app/"                # Project will be build in this directory
OBJECTS_DIR = "compile/"        # Directory for intermediate objects when compiling
MOC_DIR = $$OBJECTS_DIR/.moc
RCC_DIR = $$OBJECTS_DIR/.rrc
UI_DIR  = $$OBJECTS_DIR/.ui

INCLUDEPATH += ./src/
INCLUDEPATH +=  /usr/include

ICON    = images/Gear.icns      # Mac solution for the icon of the app
RESOURCES = images.qrc

PRECOMPILED_HEADER = src/stable.h

HEADERS = \
    src/*.h \
    src/basic_objects/*.h \
    src/graphics_objects/*.h \
    src/graphics_widgets/*.h \
    src/tool_bars/*.h

SOURCES = \
    src/*.cpp \
    src/basic_objects/*.cpp \
    src/graphics_objects/*.cpp \
    src/graphics_widgets/*.cpp \
    src/tool_bars/*.cpp
