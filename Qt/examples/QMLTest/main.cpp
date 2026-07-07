#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "helloworldbackend.h"
#include "calculatorbackend.h"
#include "converterbackend.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    HelloWorldBackend helloWorldBackend;
    CalculatorBackend calculatorBackend;
    ConverterBackend converterBackend;

    engine.rootContext()->setContextProperty("helloWorldBackend", &helloWorldBackend);
    engine.rootContext()->setContextProperty("calculatorBackend", &calculatorBackend);
    engine.rootContext()->setContextProperty("converterBackend", &converterBackend);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
