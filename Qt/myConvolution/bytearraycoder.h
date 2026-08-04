#pragma once
#include <QObject>
#include <memory>

// Кодер из QVariantList в QByteArray
class ByteArrayCoder {
public:
    enum DataType
    {
        doublePrecision,
        real,
        smallint
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
    virtual QByteArray serialize(QVariantList &data) const = 0;

    // Приведение из массива байтов
    virtual QVariantList deserialize(QByteArray &bytes) const = 0;

    // Статический метод-фабрика для создания нужного кодера
    static std::unique_ptr<ByteArrayCoder> create(DataType type, ByteOrder byteOrder);

    // Порядок записи битов
    [[nodiscard]] ByteOrder byteOrder() const;

    // Тип закодированных данных
    [[nodiscard]] DataType type() const;

private:
    ByteOrder m_byteOrder;     // Порядок записи битов
    DataType m_type;    // Тип закодированных данных

protected:
    // Установка порядка байтов
    void setSerializerByteOrder(QDataStream &serializer) const;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем double по 8 байт
class doubleByteArrayCoder : public ByteArrayCoder
{
public:
    explicit doubleByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(QVariantList &data) const override;

    QVariantList deserialize(QByteArray &bytes) const override;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем real по 4 байта
class realByteArrayCoder : public ByteArrayCoder
{
public:
    explicit realByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(QVariantList &data) const override;

    QVariantList deserialize(QByteArray &bytes) const override;
};

// Кодер из QVariantList в QByteArray, подразумевая, что кодируем smallint по 2 байта
class smallintByteArrayCoder : public ByteArrayCoder
{
public:
    explicit smallintByteArrayCoder(ByteOrder byteOrder);

    QByteArray serialize(QVariantList &data) const override;

    QVariantList deserialize(QByteArray &bytes) const override;
};
