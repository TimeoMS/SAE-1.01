TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle qt
QT -= gui

SOURCES += \
	src/main.cpp \
	src/Term/term.cpp \
	src/GameModes/gamemodes.cpp \
	src/GameModes/Matrix/matrix.cpp \
	src/GameModes/Matrix/Casali/casali.cpp

INCLUDEPATH += \
	src \
	src/Term \
	src/GameModes \
	src/GameModes/Matrix \
	src/GameModes/Matrix/Casali

LIBS += -lncurses

DEFINES += USE_LIBTERM=1
