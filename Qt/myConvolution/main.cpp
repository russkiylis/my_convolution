#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>  // setContextProperty
#include <QDebug>
#include <QSqlDatabase>

#include "backend.h"
#include "generatorbackend.h"
#include "databasemanager.h"
#include "loadgenerator.h"

// Точка входа в программу
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qDebug() << "Версия Qt:" << QT_VERSION_STR;
    qDebug() << "Пути:" << QCoreApplication::libraryPaths();
    qDebug() << "Драйвера:" << QSqlDatabase::drivers();

    // Создание объекта приложения
    QGuiApplication app(argc, argv);

    // Создаём объект классов backendа
    ConnectionBackend connectionBackend;
    GeneratorBackend generatorBackend;

    // Регистрация типа DatabaseConfiguration для передачи через сигналы/слоты
    qRegisterMetaType<DatabaseConfiguration>("DatabaseConfiguration");
    qRegisterMetaType<LoadGenerator::PostConfig>("LoadGenerator::PostConfig");
    qRegisterMetaType<LoadGenerator::PostConfig>("PostConfig");
    qRegisterMetaType<LoadGenerator::PostConfig>("PostConfig");
    qRegisterMetaType<std::vector<LoadGenerator::PostConfig>>("std::vector<PostConfig>");
    qRegisterMetaType<std::vector<LoadGenerator::PostConfig>>("std::vector<LoadGenerator::PostConfig>");
    qRegisterMetaType<LoadGenerator::DataPackage>("LoadGenerator::DataPackage");
    qRegisterMetaType<LoadGenerator::DataPackage>("DataPackage");

    // Создание QML-движка
    QQmlApplicationEngine engine;

    // Подключаем backendы в QML-движок.
    // QML начнёт видеть Q_PROPERTY и Q_INVOKABLE
    engine.rootContext()->setContextProperty("connectionBackend", &connectionBackend);
    engine.rootContext()->setContextProperty("generatorBackend", &generatorBackend);

    // Создание адреса QML-файла
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // Проверка на адекватность QML
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    // Загрузка QML в движок
    engine.load(url);

    // Непосредственно запуск приложения. Работа, пока пользователь не закроет сам
    return QGuiApplication::exec();
}
