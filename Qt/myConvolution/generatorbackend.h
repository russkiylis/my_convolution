#pragma once

#include "loadgenerator.h"
#include <QObject>

#include "postlistmodel.h"
#include "savebackend.h"

class GeneratorBackend final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PostListModel *postListModel READ postListModel CONSTANT)
    Q_PROPERTY(bool generatorEnabled READ generatorEnabled WRITE setGeneratorEnabled NOTIFY generatorEnabledChanged)
public:
    static std::vector<LoadGenerator::PostConfig> createInitialConfig();
    explicit GeneratorBackend(SaveBackend &saveBackend, QObject *parent = nullptr);
    ~GeneratorBackend() override;

    // Включён ли генератор
    [[nodiscard]] bool generatorEnabled() const;

    [[nodiscard]] std::vector<LoadGenerator::PostConfig> cfg() const;
    void setCfg(const std::vector<LoadGenerator::PostConfig> &cfg);

    // Задать статус включения генератора
    void setGeneratorEnabled(bool generatorEnabled);

    PostListModel *postListModel();

    Q_INVOKABLE void onGeneratorEnabledButtonClicked();

private:
    std::vector<LoadGenerator::PostConfig> m_cfg;
    QThread m_loadGeneratorThread;    // Поток генератора
    PostListModel m_postListModel;   // Модель для подключения к списку постов
    SaveBackend &m_saveBackend;     // Бекенд сохранения в БД

    bool m_generatorEnabled = false;     // Включён ли генератор

signals:
    void signalPostCallToggle(bool toggle);
    void generatorEnabledChanged(bool generatorEnabled);
    void signalPostConfigUpdate(std::vector<LoadGenerator::PostConfig> newConfig);

public slots:
    void slotSendData(const LoadGenerator::DataPackage & package) const;
    void slotPostCallToggle(bool toggle);
};
