#include "viewerbackend.h"
#include "trackpadorbit.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Визуализатор свёрток"));
    qmlRegisterType<TrackpadOrbit>("Synconv.Viewer", 1, 0, "TrackpadOrbit");

    ViewerBackend viewer;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("viewer"), &viewer);
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return 1;
    return app.exec();
}
