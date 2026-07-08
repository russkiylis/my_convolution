#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>  // setContextProperty

#include "backend.h"
#include "databaseconnection.h"

// Точка входа в программу
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // Создание объекта приложения
    QGuiApplication app(argc, argv);

    // Создание QML-движка
    QQmlApplicationEngine engine;

    // Создаём объект класса backend и подключаем его в QML-движок.
    // QML начнёт видеть Q_PROPERTY и Q_INVOKABLE
    Backend backend;
    engine.rootContext()->setContextProperty("backend", &backend);

    DatabaseConnection db("hi", "1.3.3.7");
    DatabaseConnection db2;
    db.openConnection();
    db2.openConnection();

    // Создание адреса QML-файла
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // Проверка на адекватность QML
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    // Загрузка QML в движок
    engine.load(url);

    // Непосредственно запуск приложения. Работа, пока пользователь не закроет сам
    return app.exec();
}
