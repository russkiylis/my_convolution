#include "bytearraycoder.h"

#include <qdatastream.h>
#include <qiodevice.h>
#include <QVariant>
#include "third-party/PackedArray/packedarray.h"

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
    default: {
        if (type > smallint && type <= pa_2b) {
            return std::make_unique<PackedArrayCoder>(byteOrder, type);
        }
        return nullptr;
    }
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

int ByteArrayCoder::bits() const {
    return m_bits;
}

std::vector<uint32_t> ByteArrayCoder::packValues(const std::vector<double> &data) const {
    // Вычисление максимума нормировки
    if (m_bits < 2 || m_bits > 8) {
        throw std::runtime_error("Недопустимое количество бит");
    }

    if (data.empty()) {
        return {};
    }

    if (data.size() > std::numeric_limits<uint32_t>::max()) {
        throw std::length_error("Слишком большое количество элементов");
    }

    const uint32_t maximum = (1u << m_bits) - 1u;

    // Нормировка массива double для упаковки
    std::vector<uint32_t> normalizedData;
    normalizedData.reserve(data.size());
    for (double const & value : data) {
        if (!std::isfinite(value))
            throw std::runtime_error("Ошибка сериализации! Невозможно записать NaN/Inf!");

        normalizedData.push_back(static_cast<uint32_t>(std::clamp(value, 0.0, 1.0) * maximum));
    }

    // Создание указателя на packedArray
    const std::unique_ptr<PackedArray, decltype(&PackedArray_destroy)> packedArrayPtr{
        PackedArray_create(m_bits, static_cast<uint32_t>(data.size())),
        &PackedArray_destroy
    };
    if (!packedArrayPtr) {
        throw std::bad_alloc{};
    }

    // Упаковка в packedArray
    PackedArray_pack(
        packedArrayPtr.get(),
        0,
        normalizedData.data(),
        static_cast<uint32_t>(normalizedData.size())
    );

    std::vector<uint32_t> result;
    result.reserve(PackedArray_bufferSize(packedArrayPtr.get()));
    for (uint32_t i = 0; i < PackedArray_bufferSize(packedArrayPtr.get()); i++) {
        result.push_back(packedArrayPtr->buffer[i]);
    }

    return result;
}

std::vector<double> ByteArrayCoder::unpackValues(const std::vector<uint32_t> &packedArray, const uint32_t elementCount) const {
    // Вычисление максимума нормировки
    if (m_bits < 2 || m_bits > 8) {
        throw std::runtime_error("Недопустимое количество бит");
    }

    const uint64_t expectedWordCount =
        (static_cast<uint64_t>(m_bits) * elementCount + 31u) / 32u;
    if (packedArray.size() != expectedWordCount) {
        throw std::logic_error("Некорректный размер упакованных данных");
    }

    if (elementCount == 0) {
        return {};
    }

    const uint32_t maximum = (1u << m_bits) - 1u;

    // Создание указателя на packedArray
    const std::unique_ptr<PackedArray, decltype(&PackedArray_destroy)> packedArrayPtr{
        PackedArray_create(m_bits, elementCount),
        &PackedArray_destroy
    };
    if (!packedArrayPtr) {
        throw std::bad_alloc{};
    }

    std::copy(packedArray.begin(), packedArray.end(), packedArrayPtr->buffer);

    std::vector<uint32_t> normalizedData(elementCount);

    // Распаковка
    PackedArray_unpack(packedArrayPtr.get(), 0, normalizedData.data(), elementCount);

    // Превращение из квантованных данных в даблы
    std::vector<double> result;
    result.reserve(normalizedData.size());
    for (auto const & value : normalizedData) {
        result.push_back(std::clamp(static_cast<double>(value)/maximum, 0.0, 1.0));
    }

    return result;
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
ByteArrayCoder(doublePrecision, byteOrder)
{
    m_bits = 8;
}

QByteArray doubleByteArrayCoder::serialize(const std::vector<double> &data) const {
    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::DoublePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    // Запись количества элементов
    const uint32_t elementCount = data.size();
    serializer << elementCount;

    for (double const & value : data) {
        if (!std::isfinite(value))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        serializer << value;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

std::vector<double> doubleByteArrayCoder::deserialize(QByteArray &bytes) const {
    std::vector<double> data;  // Данные из байтов
    if ((bytes.size() - 4) % 8 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::DoublePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    // Определение количества элементов
    uint32_t elementCount;
    serializer >> elementCount;
    data.reserve(elementCount);

    for (uint32_t i = 0; i < elementCount; i++) {
        double x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.push_back(x);
    }

    return data;
}

realByteArrayCoder::realByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(real, byteOrder)
{
    m_bits = 4;
}

QByteArray realByteArrayCoder::serialize(const std::vector<double> &data) const {
    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::SinglePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    // Запись количества элементов
    const uint32_t elementCount = data.size();
    serializer << elementCount;

    for (double const & value : data) {
        if (!std::isfinite(value))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        serializer << static_cast<float>(value);

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

std::vector<double> realByteArrayCoder::deserialize(QByteArray &bytes) const {
    std::vector<double> data;  // Данные из байтов
    if ((bytes.size() - 4) % 4 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setFloatingPointPrecision(QDataStream::SinglePrecision);     // Установка точности (количества битов)
    serializer.setVersion(QDataStream::Qt_5_15);

    // Определение количества элементов
    uint32_t elementCount;
    serializer >> elementCount;
    data.reserve(elementCount);

    for (uint32_t i = 0; i < elementCount; i++) {
        float x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.push_back(x);
    }

    return data;
}

smallintByteArrayCoder::smallintByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(smallint, byteOrder)
{
    m_bits = 2;
}

QByteArray smallintByteArrayCoder::serialize(const std::vector<double> &data) const {
    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    // Запись количества элементов
    const uint32_t elementCount = data.size();
    serializer << elementCount;

    for (double const & value : data) {
        if (!std::isfinite(value))
            throw std::logic_error("Ошибка преобразования при сериализации!");

        const int dataInt = static_cast<int>(value * 30000);

        if (dataInt > std::numeric_limits<qint16>::max() || dataInt < std::numeric_limits<qint16>::min())
            throw std::logic_error("Ошибка преобразования при сериализации! Int не поместился в qint16.");

        const auto xQInt16 = static_cast<qint16>(dataInt);
        serializer << xQInt16;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

std::vector<double> smallintByteArrayCoder::deserialize(QByteArray &bytes) const {
    std::vector<double> data;  // Данные из байтов
    if ((bytes.size() - 4) % 2 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    // Определение количества элементов
    uint32_t elementCount;
    serializer >> elementCount;
    data.reserve(elementCount);

    for (uint32_t i = 0; i < elementCount; i++) {
        qint16 x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        data.push_back(static_cast<double>(x) / 30000);
    }

    return data;
}

PackedArrayCoder::PackedArrayCoder(const ByteOrder byteOrder, const DataType type) :
ByteArrayCoder(type, byteOrder)
{
    switch (type) {
    case pa_2b:
        m_bits = 2;
        break;
    case pa_3b:
        m_bits = 3;
        break;
    case pa_4b:
        m_bits = 4;
        break;
    case pa_5b:
        m_bits = 5;
        break;
    case pa_6b:
        m_bits = 6;
        break;
    case pa_7b:
        m_bits = 7;
        break;
    case pa_8b:
        m_bits = 8;
        break;
    default:
        throw std::runtime_error("Я без понятия как это получилось.");
    }
}

QByteArray PackedArrayCoder::serialize(const std::vector<double> &data) const {

    QByteArray bytes;   // Готовые байты
    QDataStream serializer(&bytes, QIODevice::WriteOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    // Запись количества элементов
    const uint32_t elementCount = data.size();
    serializer << elementCount;

    // Получение запакованного массива
    const std::vector<uint32_t> packedArray = packValues(data);

    for (const unsigned int i : packedArray) {
        serializer << i;

        if (serializer.status() != QDataStream::Ok)
            throw std::logic_error("Ошибка сериализации!");
    }

    return bytes;
}

std::vector<double> PackedArrayCoder::deserialize(QByteArray &bytes) const {
    if (bytes.size() < 4 || (bytes.size() - 4) % 4 != 0) {
        throw std::logic_error("Некорректный размер массива байтов");
    }

    const int packedDataSize = (bytes.size() - 4) / 4;

    QDataStream serializer(&bytes, QIODevice::ReadOnly);   // Сериализатор в виде QDataStream
    setSerializerByteOrder(serializer); // Установка порядка байтов
    serializer.setVersion(QDataStream::Qt_5_15);

    // Определение количества элементов
    uint32_t elementCount;
    serializer >> elementCount;
    if (serializer.status() != QDataStream::Ok) {
        throw std::logic_error("Ошибка десериализации");
    }

    const uint64_t expectedWordCount =
        (static_cast<uint64_t>(m_bits) * elementCount + 31u) / 32u;
    if (expectedWordCount != static_cast<uint64_t>(packedDataSize)) {
        throw std::logic_error("Некорректный размер упакованных данных");
    }

    std::vector<uint32_t> packedData;
    packedData.reserve(static_cast<std::size_t>(packedDataSize));

    // Выкидываем запакованные данные из массива байтов
    for (int i = 0; i < packedDataSize; i++) {
        uint32_t x;
        serializer >> x;

        if (serializer.status() != QDataStream::Ok) {
            throw std::logic_error("Ошибка десериализации");
        }

        packedData.push_back(x);
    }

    // Распаковываем значения
    return unpackValues(packedData, elementCount);
}
