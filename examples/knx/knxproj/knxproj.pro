TEMPLATE = app
TARGET = knxproj
CONFIG += c++11 console

QT = core knx knx-private
INCLUDEPATH += .

SOURCES += main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/knx/knxproj
INSTALLS += target
