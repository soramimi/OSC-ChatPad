
DESTDIR = $$PWD/_bin

TARGET = OSC-ChatAssistant
TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
QT += core gui widgets

LIBS += -luser32 -lws2_32 -limm32

INCLUDEPATH += "C:\Qt\Tools\OpenSSLv3\Win_x64\include"
LIBS += "-LC:\Qt\Tools\OpenSSLv3\Win_x64\lib"
LIBS += -llibcrypto -llibssl

SOURCES += \
	src/Global.cpp \
	src/MainDialog.cpp \
	src/MySettings.cpp \
	src/base64.cpp \
	src/joinpath.cpp \
	src/misc.cpp \
	src/main.cpp \
	src/osc.cpp \
	src/sock.cpp \
	src/webclient.cpp

HEADERS += \
	src/Global.h \
	src/MainDialog.h \
	src/MySettings.h \
	src/base64.h \
	src/joinpath.h \
	src/jstream.h \
	src/misc.h \
	src/osc.h \
	src/sock.h \
	src/webclient.h

FORMS += \
	MainDialog.ui
