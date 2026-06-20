#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include "savecontroller.h"
extern bool initializeDatabase();

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Initialize the database connection
    if (!initializeDatabase()) {
        return -1;
    }

    // Standard Qt UI loading code
    QQmlApplicationEngine engine;

    SaveController controller;
    engine.rootContext()->setContextProperty("saveController", &controller);

    // FIX: This forces Qt to look directly for Main.qml in the root resource directory
const QUrl url(QUrl::fromLocalFile("E:/SaveVault/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}