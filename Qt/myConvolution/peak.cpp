#include "peak.h"

AbstractPeak::AbstractPeak(double const &center, double const &amplitude, QObject *parent)
    : QObject{parent},
    _center(center),
    _amplitude(amplitude)
{
}

double AbstractPeak::center() const
{
    return _center;
}

void AbstractPeak::setCenter(const double &center)
{
    _center = center;
}

double AbstractPeak::amplitude() const
{
    return _amplitude;
}

void AbstractPeak::setAmplitude(const double &amplitude)
{
    _amplitude = amplitude;
}

GaussPeak::GaussPeak(double const &center, double const &amplitude, double const &sigma, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _sigma(sigma)
{
}

AbstractPeak::PeakType GaussPeak::type() const
{
    return PeakType::Gauss;
}

double GaussPeak::valueAt(double const &deg) const
{
    //TODO: Реализация расчёта гауссовского пика
    return 0;
}

double GaussPeak::sigma() const
{
    return _sigma;
}

void GaussPeak::setSigma(double const &sigma)
{
    _sigma = sigma;
}

TrianglePeak::TrianglePeak(double const &center, double const &amplitude, double const &halfWidth, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _halfWidth(halfWidth)
{
}

AbstractPeak::PeakType TrianglePeak::type() const {
    return PeakType::Triangle;
}

double TrianglePeak::valueAt(double const &deg) const {
    //TODO: Реализация расчёта треугольного пика
    return 0;
}

double TrianglePeak::halfWidth() const {
    return _halfWidth;
}

void TrianglePeak::setHalfWidth(double const &halfWidth) {
    _halfWidth = halfWidth;
}

RectanglePeak::RectanglePeak(double const &center, double const &amplitude, double const &halfWidth, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _halfWidth(halfWidth)
{
}

AbstractPeak::PeakType RectanglePeak::type() const {
    return PeakType::Rectangle;
}

double RectanglePeak::valueAt(double const &deg) const {
    //TODO: Реализация расчёта прямоугольного пика
    return 0;
}

double RectanglePeak::halfWidth() const {
    return _halfWidth;
}

void RectanglePeak::setHalfWidth(double const &halfWidth) {
    _halfWidth = halfWidth;
}
