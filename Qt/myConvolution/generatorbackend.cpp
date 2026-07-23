#include "generatorbackend.h"
#include "loadgenerator.h"
#include "noise.h"
#include "peak.h"
//#include <cstdio>
std::vector<LoadGenerator::PostConfig> GeneratorBackend::createInitialConfig()
{
    LoadGenerator::PostConfig cfg;

    cfg.enabled = true;
    cfg.latitude = 60;
    cfg.longitude = 30;
    cfg.frequency = 100000000;
    cfg.level = 10;
    cfg.levelSigma = 1;

    cfg.minAngleH = 0;
    cfg.maxAngleH = 360;
    cfg.minAngleV = -45;
    cfg.maxAngleV = 45;

    cfg.stepH = 0.1;
    cfg.stepV = 0.1;

    cfg.minPeriod = std::chrono::milliseconds(1000);
    cfg.maxPeriod = std::chrono::milliseconds(5000);

    cfg.postName = "Пост 1";
    cfg.noiseConfig =
        std::make_unique<NormalNoise::NormalNoiseConfig>(0, 2);

    // cfg.peakConfigsH.push_back(
    //     std::make_unique<GaussPeak::GaussPeakConfig>(180, 30, 5));
    //
    // cfg.peakConfigsV.push_back(
    //     std::make_unique<GaussPeak::GaussPeakConfig>(0, 30, 5));

    std::vector<LoadGenerator::PostConfig> result;

    result.push_back(cfg);

    cfg.peakConfigsV.push_back(
        std::make_unique<RectanglePeak::RectanglePeakConfig>(30, 50, 10));
    cfg.postName = "Пост 2";
    result.push_back(cfg);

    cfg.peakConfigsV.push_back(
        std::make_unique<TrianglePeak::TrianglePeakConfig>(-30, 50, 10));
    cfg.postName = "Пост 3";
    result.push_back(cfg);

    return result;
}

GeneratorBackend::GeneratorBackend(QObject *parent)
    : QObject{parent},
    m_cfg{createInitialConfig()},
    m_postListModel(this, m_cfg)
{

    m_postListModel.setFallbackConfig(m_cfg);
    // Создаём генератор нагрузки и засовываем его в отдельный поток
    auto *loadGenerator = new LoadGenerator(m_cfg);
    loadGenerator->moveToThread(&m_loadGeneratorThread);

    //Соединение сигналов и слотов
    connect(&m_loadGeneratorThread, &QThread::finished, loadGenerator, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(loadGenerator, &LoadGenerator::signalSendData, this, &GeneratorBackend::slotSendData); // Принимаем пакет данных из генератора
    connect(loadGenerator, &LoadGenerator::signalPostCallToggle, this, &GeneratorBackend::slotPostCallToggle);  // Принимаем информацию о переключении опроса постов
    connect(this, &GeneratorBackend::signalPostCallToggle, loadGenerator, &LoadGenerator::slotPostCallToggle);  // Подаём указание переключить опрос постов
    connect(this, &GeneratorBackend::signalPostConfigUpdate, loadGenerator, &LoadGenerator::slotPostConfigUpdate);
    m_loadGeneratorThread.start();
}

GeneratorBackend::~GeneratorBackend() {
    m_loadGeneratorThread.quit();
    m_loadGeneratorThread.wait();
}

bool GeneratorBackend::generatorEnabled() const
{
    return m_generatorEnabled;
}

void GeneratorBackend::setGeneratorEnabled(const bool generatorEnabled)
{
    if (m_generatorEnabled == generatorEnabled)
        return;
    m_generatorEnabled = generatorEnabled;
    emit generatorEnabledChanged(generatorEnabled);
}

PostListModel * GeneratorBackend::postListModel() {
    return &m_postListModel;
}

void GeneratorBackend::onGeneratorEnabledButtonClicked() {
    qDebug() << "Кнопка переключения генератора нагрузки нажата.";
    setGeneratorEnabled(!m_generatorEnabled);    // Переключаем статус генератора

    // Запускаем (или останавливаем) периодический опрос постов в генераторе
    emit signalPostCallToggle(m_generatorEnabled);
}

std::vector<LoadGenerator::PostConfig> GeneratorBackend::cfg() const
{
    return m_cfg;
}

void GeneratorBackend::setCfg(const std::vector<LoadGenerator::PostConfig> &cfg)
{
    m_cfg = cfg;
}

void GeneratorBackend::slotSendData(LoadGenerator::DataPackage const &package) {
    // freopen("log.txt", "a", stdout);
    // freopen("log.txt", "a", stderr);
    qDebug().noquote().nospace()
    << "post=" << package.postName
    << "\tlevel=" << package.level
    << "\tconvV=" << package.timestamp
    << "\tconvV=" << package.convV;
}

void GeneratorBackend::slotPostCallToggle(const bool toggle) {
    setGeneratorEnabled(toggle);
}
