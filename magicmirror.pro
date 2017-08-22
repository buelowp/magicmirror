EMPLATE = app

CONFIG += gui debug core

QT += widgets

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES = MirrorFrame.cpp \
	CalendarData.cpp \
	main.cpp
		
HEADERS = MirrorFrame.h \
	CalendarData.h
