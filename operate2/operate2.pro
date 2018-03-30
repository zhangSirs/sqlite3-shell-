#-------------------------------------------------
#
# Project created by QtCreator 2018-03-29T00:07:12
#
#-------------------------------------------------

QT   += core gui
LIBS += -lsqlite3

OBJECTS_DIR += tmp
MOC_DIR     += tmp
RCC_DIR     += tmp

DESTDIR     += bin
QMAKE_CXXFLAGS += -Wno-psabi

TARGET = operate2
TEMPLATE = app


SOURCES += main.cpp\
    Driver/SqliteDB.cpp \
    operate.c

HEADERS  += \
    Driver/SqliteDB.h \
    Driver/sqlite3.h

FORMS    +=

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib
