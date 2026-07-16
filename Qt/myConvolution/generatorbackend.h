#pragma once

#include <QObject>
#include <QThread>

#include "loadGenerator.h"

class GeneratorBackend : public QObject
{
    Q_OBJECT
public:
    explicit GeneratorBackend(QObject *parent = nullptr);
    ~GeneratorBackend();

private:
    QThread loadGeneratorThread;

signals:
    void signalPostCallToggle(bool toggle);

public slots:
    void slotSendData(LoadGenerator::DataPackage const & package);
    void slotPostCallToggle(bool toggle);
};
