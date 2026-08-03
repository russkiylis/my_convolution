#include "bytearraycoder.h"

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

doubleByteArrayCoder::doubleByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(DataType::doublePrecision, byteOrder)
{
}

QByteArray doubleByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация даблов
}

QVariantList doubleByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация даблов
}

realByteArrayCoder::realByteArrayCoder(const ByteOrder byteOrder) :
ByteArrayCoder(DataType::real, byteOrder)
{
}

QByteArray realByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация реалов
}

QVariantList realByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация реалов
}

smallintByteArrayCoder::smallintByteArrayCoder(ByteOrder byteOrder) :
ByteArrayCoder(DataType::smallint, byteOrder)
{
}

QByteArray smallintByteArrayCoder::serialize(QVariantList &data) const {
    // TODO: сериализация смолинтов
}

QVariantList smallintByteArrayCoder::deserialize(QByteArray &bytes) const {
    // TODO: десериализация смолинтов
}
