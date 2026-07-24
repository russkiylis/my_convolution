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
