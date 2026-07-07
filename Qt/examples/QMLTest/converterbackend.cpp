#include <QDebug>
#include <cmath>
#include "converterbackend.h"


ConverterBackend::ConverterBackend(QObject *parent)
    : QObject{parent},
    _freq(1),
    _period(1),
    _wavelen(300)
{}

void ConverterBackend::setFreq(double const & newFreq) {
    if (_freq == newFreq)
        return;
    _freq = newFreq;
    emit freqChanged();
}

void ConverterBackend::setPeriod(double const & newPeriod) {
    if (_period == newPeriod)
        return;
    _period = newPeriod;
    emit periodChanged();
}

void ConverterBackend::setWavelen(double const & newWavelen) {
    if (_wavelen == newWavelen)
        return;
    _wavelen = newWavelen;
    emit wavelenChanged();
}

// These are called when the text is changed
void ConverterBackend::onFreqChanged(const QString & value) {
    qDebug() << "Frequency changed. Value: " << value;
    setFreq(value.toDouble());
    setPeriod(1 / freq());
    setWavelen((3 * pow(10,2)) * period());
}

void ConverterBackend::onPeriodChanged(const QString & value) {
    qDebug() << "Period changed. Value: " << value;
    setPeriod(value.toDouble());
    setFreq(1 / period());
    setWavelen((3 * pow(10,2)) * period());
}

void ConverterBackend::onWavelenChanged(const QString & value) {
    qDebug() << "Wave length changed. Value: " << value;
    setWavelen(value.toDouble());
    setFreq((3 * pow(10,2)) / wavelen());
    setPeriod(1 / freq());
}
