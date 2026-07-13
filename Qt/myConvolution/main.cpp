#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>  // setContextProperty
#include <QDebug>
#include <QSqlDatabase>

#include "backend.h"
#include "databasemanager.h"

// Точка входа в программу
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qDebug() << "Версия Qt:" << QT_VERSION_STR;
    qDebug() << "Пути:" << QCoreApplication::libraryPaths();
    qDebug() << "Драйвера:" << QSqlDatabase::drivers();

    // Создание объекта приложения
    QGuiApplication app(argc, argv);

    // Создаём объект класса backend
    Backend backend;

    // Регистрация типа DatabaseConfiguration для передачи через сигналы/слоты
    qRegisterMetaType<DatabaseConfiguration>("DatabaseConfiguration");

    // Создание QML-движка
    QQmlApplicationEngine engine;

    // Подключаем backend в QML-движок.
    // QML начнёт видеть Q_PROPERTY и Q_INVOKABLE
    engine.rootContext()->setContextProperty("backend", &backend);

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
