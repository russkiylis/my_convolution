#include "generatorbackend.h"
#include "loadGenerator.h"

GeneratorBackend::GeneratorBackend(QObject *parent)
    : QObject{parent}
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
        cfg.minPeriod = 1;
        cfg.maxPeriod = 5;
        cfg.noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(0, 0.1);
        cfg.peakConfigsH.push_back(std::make_unique<GaussPeak::GaussPeakConfig>(180, 30, 5));
        cfg.peakConfigsV.push_back(std::make_unique<GaussPeak::GaussPeakConfig>(0, 30, 5));

        std::vector<LoadGenerator::PostConfig> result;
        result.push_back(cfg);
        return result;
    };

    // Cоздаём генератор нагрузки и засовываем его в отдельный поток
    auto *loadGenerator = new LoadGenerator(initialConfig());
    loadGenerator->moveToThread(&loadGeneratorThread);

    //Соединение сигналов и слотов
    connect(&loadGeneratorThread, &QThread::finished, loadGenerator, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(loadGenerator, &LoadGenerator::signalSendData, this, &GeneratorBackend::slotSendData); // Принимаем пакет данных из генератора
    connect(loadGenerator, &LoadGenerator::signalPostCallToggle, this, &GeneratorBackend::slotPostCallToggle);  // Принимаем информацию о переключении опроса постов
    connect(this, &GeneratorBackend::signalPostCallToggle, loadGenerator, &LoadGenerator::slotPostCallToggle);  // Подаём указание переключить опрос постов
    loadGeneratorThread.start();

    // Запускаем периодический опрос постов в генераторе
    emit signalPostCallToggle(true);
}

GeneratorBackend::~GeneratorBackend() {
    loadGeneratorThread.quit();
    loadGeneratorThread.wait();
}

void GeneratorBackend::slotSendData(LoadGenerator::DataPackage const &package) {
    qDebug() << package.convH;
    qDebug() << package.convV;
}

void GeneratorBackend::slotPostCallToggle(bool toggle) {
    // TODO: Сделать какой-нибудь красивенький индикатор
}
