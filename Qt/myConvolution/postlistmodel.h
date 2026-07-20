#pragma once

#include <QAbstractListModel>
#include <QObject>

#include "loadgenerator.h"

class GeneratorBackend;

class PostListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int postIndex READ postIndex WRITE setPostIndex NOTIFY postIndexChanged)
    Q_PROPERTY(QString currentPostName READ currentPostName WRITE setCurrentPostName NOTIFY currentPostNameChanged)

    std::vector<LoadGenerator::PostConfig> &m_config;    // Ссылка на вектор PostConfig
    GeneratorBackend *m_generatorBackend;

    int m_postIndex = 0;    // Индекс выбранного поста

public:
    // Прочитать индекс поста
    int postIndex() const;

    // Установить индекс поста
    Q_INVOKABLE void setPostIndex(int postIndex);

    QString currentPostName() const;

    void setCurrentPostName(const QString &currentPostName);

    explicit PostListModel(GeneratorBackend *generatorBackend, std::vector<LoadGenerator::PostConfig> &config, QObject *parent = nullptr);

    // Подсчёт строчек
    int rowCount(const QModelIndex &parent) const override;

    // Указваем QML, как обращаться к ролям
    QHash<int, QByteArray> roleNames() const override;

    // Передаём данные в зависимости от индекса
    QVariant data(const QModelIndex &index, int role) const override;

    // Добавляем новый пост
    Q_INVOKABLE int addPost();

    // Удаляем пост
    Q_INVOKABLE int removePost(int index);

    Q_INVOKABLE void postUpdate();

private:

    // Роли для данных
    enum Role
    {
        PostNameRole = Qt::UserRole + 1,
        EnabledRole = Qt::UserRole + 2,
        MinPeriodRole = Qt::UserRole + 3,
        MaxPeriodRole = Qt::UserRole + 4
    };

signals:
    int postIndexChanged(int newPostIndex);
    QString currentPostNameChanged(QString newCurrentPostName);
};
