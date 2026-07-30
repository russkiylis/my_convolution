#pragma once
#include <qvariant.h>

class Utils {
public:
    // Преобразование double в QString без 1е-7
    static QString doubleToString(double value, int precision = 10);

    // Преобразование double в qint16
    static qint16 doubleToQint16(double value);

    // Вычленение текста из файла
    static bool fileToString(const QString& fileName, QString& result, QString* errorMessage);

    //Преобразование вектора в массив для PostgreSQL
    static QString vectorToPgArray(const std::vector<double> &values);
    static QString vectorToPgArray(const std::vector<float> &values);
    static QString vectorToPgArray(const std::vector<qint16> &values);

    //Преобразование json в vector
    static bool jsonToVector(const QVariant &json, QVector<double> &result, QString* errorMessage = nullptr);
};
