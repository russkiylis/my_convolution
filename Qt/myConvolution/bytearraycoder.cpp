#include "bytearraycoder.h"

#include <qdatastream.h>
#include <qiodevice.h>
#include <QVariant>

ByteArrayCoder::ByteArrayCoder(const DataType type, const ByteOrder byteOrder) :
m_byteOrder(byteOrder),
m_type(type)
{
}

std::unique_ptr<ByteArrayCoder> ByteArrayCoder::create(const DataType type, ByteOrder byteOrder) {
    switch (type) {
    case doublePrecision:
        return std::make_unique<doubleByteArrayCoder>(byteOrder);
    case real:
        return std::make_unique<realByteArrayCoder>(byteOrder);
    case smallint:
        return std::make_unique<smallintByteArrayCoder>(byteOrder);
    default:
        return nullptr;
    }
}

ByteArrayCoder::ByteOrder ByteArrayCoder::byteOrder() const
{
    return m_byteOrder;
}

ByteArrayCoder::DataType ByteArrayCoder::type() const
{
    return m_type;
}

void ByteArrayCoder::setSerializerByteOrder(QDataStream &serializer) const {
    switch (byteOrder()) {
    case LittleEndian:
        serializer.setByteOrder(QDataStream::LittleEndian);
        break;
    case BigEndian:
        serializer.setByteOrder(QDataStream::BigEndian);
        break;
    default:
        throw std::logic_error("Неправильный порядок байтов!");
    }
}

doubleByteArrayCoder::doubleByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(DataType::doublePrecision, byteOrder)
{
}

QByteArray doubleByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация даблов

    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::DoublePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    for (QVariant const & value : data) {
        bool ok;
        const double x = value.toDouble(&ok);
        if (!ok || !std::isfinite(x))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        serializer << x;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

QVariantList doubleByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация даблов

    QVariantList data;  // Данные из байтов
    const int dataSize = bytes.size() / 8;
    if (bytes.size() % 8 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }
    data.reserve(dataSize);

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::DoublePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    for (int i = 0; i < dataSize; i++) {
        double x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.append(x);
    }

    return data;
}

realByteArrayCoder::realByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(DataType::real, byteOrder)
{
}

QByteArray realByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация реалов

    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::SinglePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    for (QVariant const & value : data) {
        bool ok;
        const float x = value.toFloat(&ok);
        if (!ok || !std::isfinite(x))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        serializer << x;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

QVariantList realByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация реалов

    QVariantList data;  // Данные из байтов
    const int dataSize = bytes.size() / 4;
    if (bytes.size() % 4 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }
    data.reserve(dataSize);

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::SinglePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    for (int i = 0; i < dataSize; i++) {
        float x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.append(x);
    }

    return data;
}

smallintByteArrayCoder::smallintByteArrayCoder(ByteOrder byteOrder) :
ByteArrayCoder(DataType::smallint, byteOrder)
{
}

QByteArray smallintByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация смолинтов

    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    for (QVariant const & value : data) {
        bool ok;
        const double x = value.toDouble(&ok);
        if (!ok || !std::isfinite(x))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        if (std::trunc(x) != x) {
            throw std::logic_error("Значение не является целым");
        }

        if (x > std::numeric_limits<qint16>::max() || x < std::numeric_limits<qint16>::min())
            throw std::logic_error("Ошибка преобразования при сериализации! Int не поместился в qint16.");

        const auto xQInt16 = static_cast<qint16>(x);
        serializer << xQInt16;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

QVariantList smallintByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация смолинтов

    QVariantList data;  // Данные из байтов
    const int dataSize = bytes.size() / 2;
    if (bytes.size() % 2 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }
    data.reserve(dataSize);

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    for (int i = 0; i < dataSize; i++) {
        qint16 x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.append(x);
    }

    return data;
}
