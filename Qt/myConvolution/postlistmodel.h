#pragma once

#include <QAbstractListModel>
#include <QObject>
#include "loadgenerator.h"

class PostListModel : public QAbstractListModel
{
    Q_OBJECT

    std::vector<LoadGenerator::PostConfig> &_config;    // Ссылка на вектор PostConfig

public:
    explicit PostListModel(std::vector<LoadGenerator::PostConfig> &config, QObject *parent = nullptr);

    // Подсчёт строчек
    int rowCount(const QModelIndex &parent) const override;

    // Указваем QML, как обращаться к ролям
    QHash<int, QByteArray> roleNames() const override;

    // Передаём данные в зависимости от индекса
    QVariant data(const QModelIndex &index, int role) const override;

private:

    // Роли для данных
    enum Role
    {
        PostNameRole = Qt::UserRole + 1,
        EnabledRole = Qt::UserRole + 2,
        MinPeriodRole = Qt::UserRole + 3,
        MaxPeriodRole = Qt::UserRole + 4
    };
};
