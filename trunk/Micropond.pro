TEMPLATE = app
TARGET = Micropond
QT += core \
    gui \
    network
HEADERS += NetworkConfig.h \
    Outgoing.h \
    Incoming.h \
    CreatureBar.h \
    Renderer.h \
    Simulation.h \
    Window.h
SOURCES += NetworkConfig.cpp \
    Outgoing.cpp \
    Incoming.cpp \
    CreatureBar.cpp \
    Renderer.cpp \
    Simulation.cpp \
    Window.cpp \
    main.cpp
FORMS += 
RESOURCES += 
CONFIG += debug_and_release
