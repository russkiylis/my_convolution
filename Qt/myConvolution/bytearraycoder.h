#pragma once
#include <QObject>
#include <memory>
#include "third-party/PackedArray/packedarray.h"

// Кодер из QVariantList в QByteArray
class ByteArrayCoder {
public:
    enum DataType
    {
        doublePrecision,
        real,
        smallint,
        pa_2b,
        pa_3b,
        pa_4b,
        pa_5b,
        pa_6b,
        pa_7b,
        pa_8b
    };

    enum ByteOrder
    {
        LittleEndian,
        BigEndian
    };

    // Конструктор
    explicit ByteArrayCoder(DataType type, ByteOrder byteOrder);

    // Деструктор
    virtual ~ByteArrayCoder() = default;

    // Приведение к массиву байтов
    virtual QByteArray serialize(const std::vector<double> &data) const = 0;

    // Приведение из массива байтов
    virtual std::vector<double> deserialize(QByteArray &bytes) const = 0;

    // Статический метод-фабрика для создания нужного кодера
    static std::unique_ptr<ByteArrayCoder> create(DataType type, ByteOrder byteOrder);

    // Порядок записи битов
    [[nodiscard]] ByteOrder byteOrder() const;

    // Тип закодированных данных
    [[nodiscard]] DataType type() const;

    // Количество битов на элемент
    [[nodiscard]] int bits() const;

private:
    ByteOrder m_byteOrder;     // Порядок записи битов
    DataType m_type;    // Тип закодированных данных

protected:
    int m_bits{};  // Количество битов на элемент

    // Установка порядка байтов
    void setSerializerByteOrder(QDataStream &serializer) const;

    // Запаковка данных в PackedArray
    [[nodiscard]] std::vector<uint32_t> packValues(const std::vector<double> &data) const;

    // Распаковка данных из PackedArray
    [[nodiscard]] std::vector<double> unpackValues(const std::vector<uint32_t> &packedArray, uint32_t elementCount) const;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем double по 8 байт
class doubleByteArrayCoder : public ByteArrayCoder
{
public:
    explicit doubleByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(const std::vector<double> &data) const override;

    std::vector<double> deserialize(QByteArray &bytes) const override;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем real по 4 байта
class realByteArrayCoder : public ByteArrayCoder
{
public:
    explicit realByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(const std::vector<double> &data) const override;

    std::vector<double> deserialize(QByteArray &bytes) const override;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем smallint по 2 байта
class smallintByteArrayCoder : public ByteArrayCoder
{
public:
    explicit smallintByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(const std::vector<double> &data) const override;

    std::vector<double> deserialize(QByteArray &bytes) const override;
};

class PackedArrayCoder : public ByteArrayCoder
{
public:
    explicit PackedArrayCoder(ByteOrder byteOrder, DataType type);

    QByteArray serialize(const std::vector<double> &data) const override;

    std::vector<double> deserialize(QByteArray &bytes) const override;
};
