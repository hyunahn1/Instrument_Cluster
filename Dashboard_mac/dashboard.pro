# PiRacer Dashboard Qt Project File
# Qt Creator Project

QT += core gui widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PiRacerDashboard
TEMPLATE = app

CONFIG += c++17

# macOS modern SDK compatibility:
# Some Qt/qmake toolchains still inject deprecated AGL framework.
macx {
    LIBS -= -framework AGL
    LIBS += -framework OpenGL
}

# Source files
SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/widgets/SpeedometerWidget.cpp \
    src/widgets/RpmGauge.cpp \
    src/widgets/BatteryWidget.cpp \
    src/serial/SerialReader.cpp \
    src/utils/DataProcessor.cpp \
    src/utils/CalibrationManager.cpp

# Header files
HEADERS += \
    src/MainWindow.h \
    src/widgets/SpeedometerWidget.h \
    src/widgets/RpmGauge.h \
    src/widgets/BatteryWidget.h \
    src/serial/SerialReader.h \
    src/utils/DataProcessor.h \
    src/utils/CalibrationManager.h

# Resources
RESOURCES += \
    resources/dashboard.qrc

# Include paths
INCLUDEPATH += \
    src \
    src/widgets \
    src/serial \
    src/utils

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
