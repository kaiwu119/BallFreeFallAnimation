#-------------------------------------------------
#
# Project created by QtCreator 2018-05-02T18:40:49
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BallFreeFallAnimation
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    wind.cpp \
    shader.cpp

HEADERS  += widget.h \
    wind.h \
    shader.h

FORMS    += widget.ui \
    wind.ui

RESOURCES += \
    image.qrc \
    shader.qrc
win32:LIBS += -lOpengl32 \
                -lglu32 \

DISTFILES += \
    vdepthshader.vert \
    fdepthshader.frag
