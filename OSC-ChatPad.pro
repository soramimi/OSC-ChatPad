
DESTDIR = $$PWD/_bin

TARGET = jump
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
QT = core gui widgets

LIBS += -lws2_32 -limm32

SOURCES += \
	MainDialog.cpp \
	main.cpp \
	osc.cpp \
	sock.cpp

HEADERS += \
	MainDialog.h \
	osc.h \
	sock.h

FORMS += \
	MainDialog.ui
