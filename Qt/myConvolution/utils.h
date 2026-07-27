#pragma once
#include <QString>

class Utils {
public:
    // Преобразование double в QString без 1е-7
    static QString doubleToString(double value, int precision = 10);

    // Преобразование double в qint16
    static qint16 doubleToQint16(double value);
};
