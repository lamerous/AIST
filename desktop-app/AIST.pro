TEMPLATE = app
TARGET = AIST
INCLUDEPATH += . headers src ui
DEPENDPATH += . headers src ui

QT += core widgets network

HEADERS += headers/*.h
FORMS += ui/*.ui
SOURCES += src/*.cpp
RESOURCES += resources.qrc

OBJECTS_DIR = build/obj
UI_DIR = build/ui
MOC_DIR = build/moc
DESTDIR = build/bin
