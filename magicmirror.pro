EMPLATE = app

CONFIG += gui network debug core

QT += widgets network

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES = MirrorFrame.cpp \
	CalendarData.cpp \
	WeatherData.cpp \
	main.cpp
		
HEADERS = MirrorFrame.h \
	CalendarData.h \
	WeatherData.h