#pragma once

#include <QAbstractListModel>
#include <QObject>

#include "loadgenerator.h"

class GeneratorBackend;

enum AngleStep
{
    _1,
    _05,
    _02,
    _01,
    _001
};

class PostListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int postIndex READ postIndex WRITE setPostIndex NOTIFY postIndexChanged)

    Q_PROPERTY(QString currentPostName READ currentPostName WRITE setCurrentPostName NOTIFY currentPostNameChanged)
    Q_PROPERTY(QString currentLatitude READ currentLatitude WRITE setCurrentLatitude NOTIFY currentLatitudeChanged)
    Q_PROPERTY(QString currentLongitude READ currentLongitude WRITE setCurrentLongitude NOTIFY currentLongitudeChanged)
    Q_PROPERTY(QString currentFrequency READ currentFrequency WRITE setCurrentFrequency NOTIFY currentFrequencyChanged)
    Q_PROPERTY(QString currentLevel READ currentLevel WRITE setCurrentLevel NOTIFY currentLevelChanged)
    Q_PROPERTY(QString currentLevelSigma READ currentLevelSigma WRITE setCurrentLevelSigma NOTIFY currentLevelSigmaChanged)
    Q_PROPERTY(QString currentMinAngleH READ currentMinAngleH WRITE setCurrentMinAngleH NOTIFY currentMinAngleHChanged)
    Q_PROPERTY(QString currentMaxAngleH READ currentMaxAngleH WRITE setCurrentMaxAngleH NOTIFY currentMaxAngleHChanged)
    Q_PROPERTY(int currentStepH READ currentStepH WRITE setCurrentStepH NOTIFY currentStepHChanged)
    Q_PROPERTY(QString currentMinAngleV READ currentMinAngleV WRITE setCurrentMinAngleV NOTIFY currentMinAngleVChanged)
    Q_PROPERTY(QString currentMaxAngleV READ currentMaxAngleV WRITE setCurrentMaxAngleV NOTIFY currentMaxAngleVChanged)
    Q_PROPERTY(int currentStepV READ currentStepV WRITE setCurrentStepV NOTIFY currentStepVChanged)
    Q_PROPERTY(QString currentMinPeriod READ currentMinPeriod WRITE setCurrentMinPeriod  NOTIFY currentMinPeriodChanged)
    Q_PROPERTY(QString currentMaxPeriod READ currentMaxPeriod WRITE setCurrentMaxPeriod  NOTIFY currentMaxPeriodChanged)
    Q_PROPERTY(bool currentPostEnabled READ currentPostEnabled WRITE setCurrentPostEnabled NOTIFY currentPostEnabledChanged)

    std::vector<LoadGenerator::PostConfig> &m_config;    // Ссылка на вектор PostConfig
    std::vector<LoadGenerator::PostConfig> m_fallbackConfig;   // Вектор конфигов, к которому мы сможем откатиться
public:
    void setFallbackConfig(const std::vector<LoadGenerator::PostConfig> &fallbackConfig);

private:
    GeneratorBackend *m_generatorBackend;

    int m_postIndex = 0;    // Индекс выбранного поста

public:
    // Прочитать индекс поста
    int postIndex() const;

    // Установить индекс поста
    Q_INVOKABLE void setPostIndex(int postIndex);

    // Выдать имя поста по текущему индексу
    QString currentPostName() const;

    // Установить имя поста по текущему индексу
    void setCurrentPostName(const QString &currentPostName);

    // Выдать широту по текущему индексу
    QString currentLatitude() const;

    // Установить широту по текущему индексу
    void setCurrentLatitude(const QString &currentLatitude);

    // Выдать долготу по текущему индексу
    QString currentLongitude() const;

    // Установить долготу по текущему индексу
    void setCurrentLongitude(const QString &currentLongitude);

    // Выдать частоту по текущему индексу
    QString currentFrequency() const;

    // Установить частоту по текущему индексу
    void setCurrentFrequency(const QString &frequency);

    // Выдать средний уровень по текущему индексу
    QString currentLevel() const;

    // Установить средний уровень по текущему индексу
    void setCurrentLevel(const QString &currentLevel);

    // Выдать СКО уровня по текущему индексу
    QString currentLevelSigma() const;

    // Установить СКО уровня по текущему индексу
    void setCurrentLevelSigma(const QString &currentLevelSigma);

    // Выдать минимальный горизонтальный угол по текущему индексу
    QString currentMinAngleH() const;

    // Установить минимальный горизонтальный угол по текущему индексу
    void setCurrentMinAngleH(const QString &currentMinAngleH);

    // Выдать максимальный горизонтальный угол по текущему индексу
    QString currentMaxAngleH() const;

    // Установить максимальный горизонтальный угол по текущему индексу
    void setCurrentMaxAngleH(const QString &currentMaxAngleH);

    // Выдать шаг горизонтального угла
    int currentStepH() const;

    // Установить шаг горизонтального угла
    void setCurrentStepH(int currentStepH);

    // Выдать минимальный вертикальный угол по текущему индексу
    QString currentMinAngleV() const;

    // Установить минимальный вертикальный угол по текущему индексу
    void setCurrentMinAngleV(const QString &currentMinAngleV);

    // Выдать максимальный вертикальный угол по текущему индексу
    QString currentMaxAngleV() const;

    // Установить максимальный вертикальный угол по текущему индексу
    void setCurrentMaxAngleV(const QString &currentMaxAngleV);

    // Выдать шаг вертикального угла
    int currentStepV() const;

    // Установить шаг горизонтального угла
    void setCurrentStepV(int currentStepV);

    // Выдать минимальный период генерации
    QString currentMinPeriod() const;

    // Установить минимальный период генерации
    void setCurrentMinPeriod(const QString &currentMinPeriod);

    // Выдать максимальный период генерации
    QString currentMaxPeriod() const;

    // Установить максимальный период генерации
    void setCurrentMaxPeriod(const QString &currentMaxPeriod);

    // Выдать статус активности поста
    bool currentPostEnabled() const;

    // Установить статус активности поста
    void setCurrentPostEnabled(bool currentPostEnabled);

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

    // Обновляем посты в генераторе (сохраняем изменения)
    Q_INVOKABLE void postUpdate();

    // Отменяем изменения
    Q_INVOKABLE void fallback();

private:

    // Роли для данных
    enum Role
    {
        PostNameRole = Qt::UserRole + 1,
        EnabledRole = Qt::UserRole + 2,
        MinPeriodRole = Qt::UserRole + 3,
        MaxPeriodRole = Qt::UserRole + 4
    };

    // В одной функции все нужные емиты
    void emits();

signals:
    void postIndexChanged(int newPostIndex);
    void currentPostNameChanged(QString newCurrentPostName);
    void currentLatitudeChanged(QString newCurrentLatidude);
    void currentLongitudeChanged(QString newCurrentLongitude);
    void currentFrequencyChanged(QString newCurrentFrequency);
    void currentLevelChanged(QString newCurrentLevel);
    void currentLevelSigmaChanged(QString newCurrentLevelSigma);
    void currentMinAngleHChanged(QString newCurrentMinAngleH);
    void currentMaxAngleHChanged(QString newCurrentMaxAngleH);
    void currentStepHChanged(int newCurrentStepH);
    void currentMinAngleVChanged(QString newCurrentMinAngleV);
    void currentMaxAngleVChanged(QString newCurrentMaxAngleV);
    void currentStepVChanged(int newCurrentStepV);
    void currentMinPeriodChanged(QString newCurrentMinPeriod);
    void currentMaxPeriodChanged(QString newCurrentMaxPeriod);
    void currentPostEnabledChanged(bool newPostEnabled);
};
