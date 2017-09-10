TEMPLATE = app

CONFIG += gui network debug core

QT += widgets network

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES = MirrorFrame.cpp \
	CalendarData.cpp \
	WeatherData.cpp \
	MotionDetect.cpp \
	ds18b20.cpp \
	main.cpp
		
HEADERS = MirrorFrame.h \
	CalendarData.h \
	WeatherData.h \
	MotionDetect.h \
	ds18b20.h
