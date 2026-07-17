#pragma once

#include "loadGenerator.h"
#include <QObject>

class GeneratorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool generatorEnabled READ generatorEnabled WRITE setGeneratorEnabled NOTIFY generatorEnabledChanged)
public:
    explicit GeneratorBackend(QObject *parent = nullptr);
    ~GeneratorBackend() override;

    // Включён ли генератор
    [[nodiscard]] bool generatorEnabled() const;

    // Задать статус включения генератора
    void setGeneratorEnabled(bool generatorEnabled);

    Q_INVOKABLE void onGeneratorEnabledButtonClicked();

private:
    QThread loadGeneratorThread;    // Поток генератора

    bool _generatorEnabled = false;     // Включён ли генератор

signals:
    void signalPostCallToggle(bool toggle);
    void generatorEnabledChanged(bool generatorEnabled);

public slots:
    void slotSendData(LoadGenerator::DataPackage const & package);
    void slotPostCallToggle(bool toggle);
};
