#-------------------------------------------------
#
# Project created by QtCreator 2012-02-16T11:35:32
#
#-------------------------------------------------

TARGET = EksScript
TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include("../EksCore/GeneralOptions.pri")

SOURCES += \
    XInterface.cpp \
    XScriptException.cpp \
    XScriptValue.cpp \
    XScriptFunction.cpp \
    XScriptSource.cpp \
    XScriptObject.cpp \
    XQObjectWrapper.cpp \
    XFunctions.cpp \
    XScriptEngine.cpp \
    XQtWrappers.cpp \
    Engines/XScriptDartEngine.cpp \
    Engines/XScriptJavascriptEngine.cpp \
    XScriptInterfaceBase.cpp

HEADERS += XScriptGlobal.h \
    XInterface.h \
    XSignature.h \
    XSignatureHelpers.h \
    XSignatureSpecialisations.h \
    XTemplateMetaHelpers.h \
    XScriptTypeInfo.h \
    XConvertToScript.h \
    XConvertFromScript.h \
    XConvertScriptSTL.h \
    XConvert.h \
    XScriptException.h \
    XScriptValue.h \
    XScriptFunction.h \
    XInterfaceUtilities.h \
    XProperties.h \
    XScriptConstructors.h \
    XConvertToScriptMap.h \
    XFunctions.h \
    XFunctionSpecialisations.h \
    XScriptSource.h \
    XScriptObject.h \
    XQObjectWrapper.h \
    XScriptEngine.h \
    XQtWrappers.h \
    XScriptDefinitions.h \
    XScriptInterfaceBase.h

LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore/include ./

#QT += v8-private
#define X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT

#win32-msvc2010 {
#  DEFINES += X_SCRIPT_ENGINE_ENABLE_DART
#  LIBS += -L./dart/ -llibdart_builtin -llibdart_withcore -llibdart_lib_withcore -llibdouble_conversion -llibdart_vm -llibjscre
#}

OTHER_FILES += \
    License.txt
