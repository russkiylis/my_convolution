#pragma once
#include <qabstractitemmodel.h>

#include "loadgenerator.h"

class PeakListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit PeakListModel(std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &peakConfig, QObject *parent = nullptr);

    // Подсчёт строчек
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    // Указваем QML, как обращаться к ролям
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // Передаём данные в зависимости от индекса
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    // // Обновляем поля в QML (не модель, а именно поля) (не пригодилось)
    // void qmlUpdate();

private:
    enum Roles
    {
        PeakTypeRole = Qt::UserRole + 1,
        CenterRole,
        AmplitudeRole,
        SigmaRole,
        HalfWidthRole
    };

    std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> *m_peakConfig;     // Конфигурация пиков (Указатель меняет адрес в зависимости от выбранного поста)

public:
    // Получить ссылку на конфиг пиков
    [[nodiscard]] std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &peakConfig() const;

    // Задать конфиг пиков (вызывать всегда при смене PostIndex или изменении размера вектора постов)
    void setPeakConfig(std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> &peakConfig);

    Q_INVOKABLE void setPeakType(int row, int peakType);
    Q_INVOKABLE void setCenter(int row, double center);
    Q_INVOKABLE void setAmplitude(int row, double amplitude);
    Q_INVOKABLE void setSigma(int row, double sigma);
    Q_INVOKABLE void setHalfWidth(int row, double halfWidth);
    Q_INVOKABLE int addPeak();
    Q_INVOKABLE int removePeak(int row);
};
