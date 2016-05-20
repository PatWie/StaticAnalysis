#-------------------------------------------------
#
# Project created by QtCreator 2014-05-03T16:06:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TIA
TEMPLATE = app

win32:RC_ICONS += images/icon.ico

SOURCES += main.cpp\
        tia.cpp \
    Helpers/HString.cpp \
    tia_slots.cpp \
    Models/MTarget.cpp \
    Models/MDB.cpp \
    Models/MStaticDisassembler.cpp \
    Helpers/HStyle.cpp \
    Views/VCode.cpp \
    Views/VJump.cpp \
    Helpers/HArrow.cpp \
    Helpers/HDockWidgetIcon.cpp \
    Dialogs/DAbout.cpp \
    Helpers/HInstruction.cpp \
    Models/MPatches.cpp \
    Helpers/HTokenizer.cpp \
    Views/VLog.cpp \
    Models/MBreakpoints.cpp \
    Widgets/WBreakpoints.cpp \
    Models/MFunctions.cpp \
    Widgets/WFunctions.cpp \
    Widgets/WDisassembly.cpp \
    Models/MImports.cpp \
    Widgets/WImports.cpp \
    Models/MSegments.cpp \
    Views/VImports.cpp \
    Widgets/WSegments.cpp \
    Models/MLabels.cpp \
    Widgets/WLabels.cpp \
    Models/MComments.cpp \
    Widgets/WComments.cpp \
    Widgets/WDebugger.cpp

HEADERS  += tia.h \
    Helpers/HString.h \
    Global.h \
    Models/MTarget.h \
    Models/MDB.h \
    BeaEngine/basic_types.h \
    BeaEngine/BeaEngine.h \
    BeaEngine/export.h \
    BeaEngine/macros.h \
    Models/MStaticDisassembler.h \
    Helpers/HStyle.h \
    Views/VCode.h \
    Views/VJump.h \
    Helpers/HArrow.h \
    Helpers/HDockWidgetIcon.h \
    Dialogs/DAbout.h \
    Helpers/HInstruction.h \
    Models/MPatches.h \
    Views/VCode.h \
    Helpers/HTokenizer.h \
    Views/VLog.h \
    Models/MBreakpoints.h \
    Widgets/WBreakpoints.h \
    Models/MFunctions.h \
    Widgets/WFunctions.h \
    Widgets/WDisassembly.h \
    Models/MImports.h \
    Widgets/WImports.h \
    Models/MSegments.h \
    Views/VImports.h \
    Widgets/WSegments.h \
    Widgets/WLabels.h \
    Models/MLabels.h \
    Models/MComments.h \
    Widgets/WComments.h \
    Widgets/WDebugger.h

FORMS    += tia.ui \
    Dialogs/DAbout.ui

LIBS += -L$$PWD/BeaEngine/ -lBeaEngine_x86

INCLUDEPATH += $$PWD/BeaEngine
DEPENDPATH += $$PWD/BeaEngine

RESOURCES += \
    res.qrc
