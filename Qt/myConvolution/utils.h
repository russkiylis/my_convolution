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

    // Сжимание вектора
    template <typename T>
    static QVector<T> shrinkVector(const QVector<T> &values, const int newSize) {

        if (!std::is_arithmetic_v<T>)
            throw std::logic_error("Сжатие вектора не поддерживается для не-чисел.");

        if (newSize <= 0)
            throw std::logic_error("Сомнительная идея - делать вектор нулевым или меньше нуля.");

        // Если изначально всунули вектор меньше целевого
        if (values.size() <= newSize)
            return values;

        QVector<T> result;
        result.reserve(newSize);

        for (int i = 0; i < newSize; ++i) {
            const int startIndex = i * values.size() / newSize;
            const int endIndex   = (i + 1) * values.size() / newSize;

            const auto startIt = values.begin() + startIndex;
            const auto endIt = values.begin() + endIndex;
            auto maxIt = std::max_element(startIt, endIt);  // Адрес максимума среди отрезка
            result.push_back(*maxIt);
        }

        return result;
    }
};
