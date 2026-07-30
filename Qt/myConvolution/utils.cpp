#include "utils.h"

#include <qfile.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
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

QString Utils::vectorToPgArray(const std::vector<double> &values)
{
    QString result = "{";

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += doubleToString(values[i], 17);    // TODO: Уточнить точность дабла
    }

    result += "}";
    return result;
}

QString Utils::vectorToPgArray(const std::vector<float> &values) {
    QString result = "{";

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += doubleToString(values[i], 9);     // TODO: Уточнить точность флоата
    }

    result += "}";
    return result;
}

QString Utils::vectorToPgArray(const std::vector<qint16> &values) {
    QString result = "{";

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += QString::number(values[i]);
    }

    result += "}";
    return result;
}

bool Utils::jsonToVector(const QVariant &json, QVector<double> &result, QString *errorMessage) {
    result.clear();
    if (errorMessage != nullptr)
        errorMessage->clear();

    if (!json.isValid() || json.isNull()) {
        if (errorMessage != nullptr)
            *errorMessage = "Получен пустой массив";
        return false;
    }

    QJsonParseError jsonParseError{};
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toString().toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError) {
        if (errorMessage != nullptr)
            *errorMessage = jsonParseError.errorString();
        return false;
    }

    if (!jsonDocument.isArray())
    {
        if (errorMessage != nullptr)
            *errorMessage = "Полученный json не массив!";
        return false;
    }

    const QJsonArray jsonArray = jsonDocument.array();
    result.reserve(jsonArray.size());

    for (QJsonValue element : jsonArray) {
        if (!element.isDouble()) {
            if (errorMessage != nullptr)
                *errorMessage = "В json-массиве есть не числа!";
            result.clear();
            return false;
        }
        result.push_back(element.toDouble());
    }

    return true;
}
