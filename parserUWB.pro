QT += gui widgets

QT += core network charts
QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        calibration.cpp \
        map/map.cpp\
        kx_pult/kx_protocol.cpp \
        kx_pult/qkx_coeffs.cpp \
        kx_pult/qpiconfig.cpp \
        main.cpp \
        ProtocolUWB.cpp \
        TrilatUWB.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    calibration.h \
    map/map.h \
    kx_pult/kx_protocol.h \
    kx_pult/qkx_coeffs.h \
    kx_pult/qpiconfig.h \
    ProtocolUWB.h \
    TrilatUWB.h

FORMS += map/map.ui
