#ifndef CONVERTERBACKEND_H
#define CONVERTERBACKEND_H

#include <QObject>
#include <QString>

class ConverterBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double freq READ freq WRITE setFreq NOTIFY freqChanged)
    Q_PROPERTY(double period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(double wavelen READ wavelen WRITE setWavelen NOTIFY wavelenChanged)

public:
    explicit ConverterBackend(QObject *parent = nullptr);

    double freq() {return _freq;}
    double period() {return _period;}
    double wavelen() {return _wavelen;}

    void setFreq(double const & newFreq);
    void setPeriod(double const & newPeriod);
    void setWavelen(double const & newWavelen);

    Q_INVOKABLE void onFreqChanged(const QString & value);
    Q_INVOKABLE void onPeriodChanged(const QString & value);
    Q_INVOKABLE void onWavelenChanged(const QString & value);

private:
    double _freq;
    double _period;
    double _wavelen;

signals:
    void freqChanged();
    void periodChanged();
    void wavelenChanged();

};

#endif // CONVERTERBACKEND_H
