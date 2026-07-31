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

    double qualityH;
    double qualityV;
};

struct DataForVisualization
{
    RowForVisualization row;
    QVector<double> convH;
    QVector<double> convV;
};
