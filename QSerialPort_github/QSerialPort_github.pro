QT       += core gui serialport charts sql serialport

#LIBS += E:\task\measure_system\QT\QSerialPort_github

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include(./CustomPlot/CustomPlot.pri)

LIBS += -L$$PWD \
        -llibfftw3-3 \
        -llibfftw3f-3 \
        -llibfftw3l-3

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mythread.cpp \
    qcustomplot.cpp \
    qmysql.cpp

HEADERS += \
    fftw3.h \
    mainwindow.h \
    mythread.h \
    qcustomplot.h \
    qmysql.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
