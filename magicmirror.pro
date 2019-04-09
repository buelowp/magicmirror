TEMPLATE = app

CONFIG += gui network debug core

USE_RPI = FALSE

QT += widgets network qmqtt

OBJECTS_DIR = .obj
MOC_DIR = .moc

defined (__USE_RPI__, var) {
     LIBS = -lwiringPi -lth02
    QMAKE_CXXFLAGS += -D__USE_RPI__
}

SOURCES = MirrorFrame.cpp \
	CalendarData.cpp \
	WeatherData.cpp \
	main.cpp \
	weathericon.cpp \
	qmqttsubscriber.cpp
		
HEADERS = MirrorFrame.h \
	CalendarData.h \
	WeatherData.h \
	weathericon.h \
	qmqttsubscriber.h

