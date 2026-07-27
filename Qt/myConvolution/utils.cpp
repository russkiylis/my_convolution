#include "utils.h"

#include <qfile.h>
#include <QString>

QString Utils::doubleToString(const double value, const int precision)
{
    QString result = QString::number(value, 'f', precision);

    if (result.contains('.')) {
        while (result.endsWith('0')) {
            result.chop(1);
        }

        if (result.endsWith('.')) {
            result.chop(1);
        }
    }

    return result;
}

qint16 Utils::doubleToQint16(const double value) {
    constexpr double maximum = 30000.0;

    return static_cast<qint16>(
        std::lround(std::clamp(value, -1.0, 1.0) * maximum)
    );
}

bool Utils::fileToString(const QString& fileName, QString &result, QString *errorMessage = nullptr) {
    QFile file(fileName);

    // Если файл адекватно не открылся, возвращаем строчку с ошибкой и false
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage != nullptr)
            *errorMessage = file.errorString();
        return false;
    }

    result = QString::fromUtf8(file.readAll());     // Возвращаем текст из файла
    file.close();
    return true;
}

QString Utils::vectorToPgDoubleArray(const std::vector<double> &values)
{
    QString result = "{";

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += doubleToString(values[i], 17);
    }

    result += "}";
    return result;
}
