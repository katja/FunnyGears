QT  +=  core gui \
        widgets \               # Without this line Qt won't recognise any QtWidgets!
        opengl widgets          # specify all modules used in the project here!

# Specifies project configuration and compiler options
# Possible values include:  debug, release, dubug_and_release,
#                           ordered (=> SUBDIRS listed are processed in given order)
#                           warn_on, warn_off (=> print all/fewest possible compiler warnings)
CONFIG  += debug                # Only during development necessary: Print debugging symbols and information

DESTDIR = "app/"                # Project will be build in this directory
OBJECTS_DIR = "compile/"        # Directory for intermediate objects when compiling
MOC_DIR = $$OBJECTS_DIR/.moc
RCC_DIR = $$OBJECTS_DIR/.rrc
UI_DIR  = $$OBJECTS_DIR/.ui

ICON    = images/Gear.icns      # Mac solution for the icon of the app

PRECOMPILED_HEADER = src/stable.h

HEADERS = \
    src/*.h \

SOURCES = \
    src/*.cpp \
