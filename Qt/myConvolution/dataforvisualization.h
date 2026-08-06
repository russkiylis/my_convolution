#pragma once
#include <QDateTime>

// Отображаемая строчка (без спарклайна свёрток)
struct RowForVisualization
{
    qint64 id;
    QDateTime timestamp;

    QString sysname;
    double azimuth;
    double elevation;
    double power;
    double frequency;
    double latitude;
    double longitude;

    QString dataType;

    double quality;
    int countH;
    int countV;
};

struct DataForVisualization
{
    QVector<RowForVisualization> rows;
    QVector<QVector<double>> convs;
};
