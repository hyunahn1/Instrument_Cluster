/**
 * @file main.cpp
 * @brief PiRacer Dashboard Application Entry Point
 * @author Ahn Hyunjun
 * @date 2026-02-16
 * @version 1.0.0
 */

#include <QApplication>
#include <QtGlobal>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Qt 5 compatibility: these attributes are deprecated in Qt 6.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    
    // Application metadata
    app.setApplicationName("PiRacer Dashboard");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PiRacer");
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}
