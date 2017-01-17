TARGET = QtKnx

CONFIG += c++11
QT_FOR_PRIVATE = network

QMAKE_DOCS = $$PWD/doc/qtknx.qdocconf

PUBLIC_HEADERS += \
    qknxglobal.h \
    qknxaddress.h

PRIVATE_HEADERS +=

SOURCES += \
    qknxaddress.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
