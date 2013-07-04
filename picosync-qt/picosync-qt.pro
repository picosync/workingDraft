#-------------------------------------------------
#
# Project created by QtCreator 2013-06-18T15:18:09
#
#-------------------------------------------------

QT	+= core network
QT	-= gui

TARGET = picosync-qt
TEMPLATE = app

LIBS += \
	-ltorrent-rasterbar \
	-lqca \
	-lboost_system \
	-l:../ext/cyoencode-1.0.2/libcyoencode.a \
	-l:../ext/libutp/libutp.a
INCLUDEPATH += ../ext/

SOURCES += \
	src/main.cpp \
	src/Discovery.cpp \
	src/Secret.cpp \
	src/Peer.cpp \
	src/PeerAddress.cpp \
	src/Main.cpp

HEADERS  += \
	src/Discovery.h \
	src/Secret.h \
	src/Peer.h \
	src/PeerAddress.h \
	src/Main.h


# define build directory
DESTDIR = bin

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build
UI_DIR = build

