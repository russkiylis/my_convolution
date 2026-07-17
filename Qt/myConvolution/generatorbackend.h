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

    // Задать статус включения генератора
    void setGeneratorEnabled(bool generatorEnabled);

    PostListModel *postListModel();

    Q_INVOKABLE void onGeneratorEnabledButtonClicked();

private:
    QThread loadGeneratorThread;    // Поток генератора
    PostListModel _postListModel;   // Модель для подключения к списку постов

    bool _generatorEnabled = false;     // Включён ли генератор
    std::vector<LoadGenerator::PostConfig> _cfg;

signals:
    void signalPostCallToggle(bool toggle);
    void generatorEnabledChanged(bool generatorEnabled);

public slots:
    void slotSendData(LoadGenerator::DataPackage const & package);
    void slotPostCallToggle(bool toggle);
};
