#pragma once

#include "loadgenerator.h"
#include <QObject>

#include "postlistmodel.h"

class GeneratorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PostListModel *postListModel READ postListModel CONSTANT)
    Q_PROPERTY(bool generatorEnabled READ generatorEnabled WRITE setGeneratorEnabled NOTIFY generatorEnabledChanged)
public:
    explicit GeneratorBackend(QObject *parent = nullptr);
    ~GeneratorBackend() override;

    // Включён ли генератор
    [[nodiscard]] bool generatorEnabled() const;

    std::vector<LoadGenerator::PostConfig> cfg() const;
    void setCfg(const std::vector<LoadGenerator::PostConfig> &cfg);

    // Задать статус включения генератора
    void setGeneratorEnabled(bool generatorEnabled);

    PostListModel *postListModel();

    Q_INVOKABLE void onGeneratorEnabledButtonClicked();

private:
    std::vector<LoadGenerator::PostConfig> m_cfg;
    QThread m_loadGeneratorThread;    // Поток генератора
    PostListModel m_postListModel;   // Модель для подключения к списку постов

    bool m_generatorEnabled = false;     // Включён ли генератор

signals:
    void signalPostCallToggle(bool toggle);
    void generatorEnabledChanged(bool generatorEnabled);
    void signalPostConfigUpdate(std::vector<LoadGenerator::PostConfig> newConfig);

public slots:
    void slotSendData(LoadGenerator::DataPackage const & package);
    void slotPostCallToggle(bool toggle);
};
