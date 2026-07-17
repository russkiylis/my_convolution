#include "generatorbackend.h"
#include "loadgenerator.h"
#include "noise.h"
#include "peak.h"
//#include <cstdio>

GeneratorBackend::GeneratorBackend(QObject *parent)
    : QObject{parent},
    _postListModel(_cfg, this)
{
    // Лямбда для создания начального конфига нагрузки
    auto initialConfig = []() -> std::vector<LoadGenerator::PostConfig>
    {
        LoadGenerator::PostConfig cfg;

        // Это нужно чтобы можно было легко поменять начальный конфиг
        // Потом оно всё уйдет в комменты или вообще исчезнет
        cfg.enabled = true;
        cfg.latitude = 60;
        cfg.longitude = 30;
        cfg.frequency = 100000000;
        cfg.level = 10;
        cfg.levelSigma = 1;
        cfg.minAngleH = 0;
        cfg.minAngleV = -45;
        cfg.maxAngleH = 360;
        cfg.maxAngleV = 45;
        cfg.stepH = 0.1;
        cfg.stepV = 0.1;
        cfg.minPeriod = std::chrono::milliseconds(20);
        cfg.maxPeriod = std::chrono::milliseconds(100);
        cfg.postName = "Пост 1";
        // cfg.noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(0, 2);
        cfg.noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(-10, 20);
        cfg.peakConfigsH.push_back(std::make_unique<GaussPeak::GaussPeakConfig>(180, 30, 5));
        cfg.peakConfigsV.push_back(std::make_unique<GaussPeak::GaussPeakConfig>(0, 30, 5));

        std::vector<LoadGenerator::PostConfig> result;
        result.push_back(cfg);
        cfg.peakConfigsV.push_back(std::make_unique<RectanglePeak::RectanglePeakConfig>(30, 50, 10));
        cfg.postName = "Пост 2";
        result.push_back(cfg);
        cfg.peakConfigsV.push_back(std::make_unique<TrianglePeak::TrianglePeakConfig>(-30, 50, 10));
        cfg.postName = "Пост 3";
        result.push_back(cfg);
        cfg.noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(0, 3);
        for (int i = 4; i < 101; i++) {
            cfg.postName = "Пост " + QString::number(i);
            cfg.enabled = i % 2;
            result.push_back(cfg);
        }
        return result;
    };
    _cfg = initialConfig();
    // Создаём генератор нагрузки и засовываем его в отдельный поток
    auto *loadGenerator = new LoadGenerator(_cfg);
    loadGenerator->moveToThread(&loadGeneratorThread);

    //Соединение сигналов и слотов
    connect(&loadGeneratorThread, &QThread::finished, loadGenerator, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(loadGenerator, &LoadGenerator::signalSendData, this, &GeneratorBackend::slotSendData); // Принимаем пакет данных из генератора
    connect(loadGenerator, &LoadGenerator::signalPostCallToggle, this, &GeneratorBackend::slotPostCallToggle);  // Принимаем информацию о переключении опроса постов
    connect(this, &GeneratorBackend::signalPostCallToggle, loadGenerator, &LoadGenerator::slotPostCallToggle);  // Подаём указание переключить опрос постов
    loadGeneratorThread.start();
}

GeneratorBackend::~GeneratorBackend() {
    loadGeneratorThread.quit();
    loadGeneratorThread.wait();
}

bool GeneratorBackend::generatorEnabled() const
{
    return _generatorEnabled;
}

void GeneratorBackend::setGeneratorEnabled(const bool generatorEnabled)
{
    _generatorEnabled = generatorEnabled;
    emit generatorEnabledChanged(generatorEnabled);
}

PostListModel * GeneratorBackend::postListModel() {
    return &_postListModel;
}

void GeneratorBackend::onGeneratorEnabledButtonClicked() {
    qDebug() << "Кнопка переключения генератора нагрузки нажата.";
    setGeneratorEnabled(!_generatorEnabled);    // Переключаем статус генератора

    // Запускаем (или останавливаем) периодический опрос постов в генераторе
    emit signalPostCallToggle(_generatorEnabled);
}

void GeneratorBackend::slotSendData(LoadGenerator::DataPackage const &package) {
    // freopen("log.txt", "a", stdout);
    // freopen("log.txt", "a", stderr);
    // qDebug().noquote().nospace()
    // << "post=" << package.postName
    // << "\tlevel=" << package.level
    // << "\tconvV=" << package.timestamp
    // << "\tconvV=" << package.convV;
}

void GeneratorBackend::slotPostCallToggle(const bool toggle) {
    setGeneratorEnabled(toggle);
}
