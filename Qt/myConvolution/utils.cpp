#include "utils.h"

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
