#ifndef CALCULATORBACKEND_H
#define CALCULATORBACKEND_H

#include <QObject>

enum CalcOperation {
    add,
    substract,
    multiply,
    divide
};

class CalculatorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int firstValue READ firstValue WRITE setFirstValue NOTIFY firstValueChanged)
    Q_PROPERTY(int secondValue READ secondValue WRITE setSecondValue NOTIFY secondValueChanged)
    Q_PROPERTY(double result READ result WRITE setResult NOTIFY resultChanged)
    Q_PROPERTY(CalcOperation calcOperation READ calcOperation WRITE setCalcOperation NOTIFY calcOperationChanged)

public:
    explicit CalculatorBackend(QObject *parent = nullptr);

    int firstValue() const {return _firstValue;}
    int secondValue() const {return _secondValue;}
    double result() const {return _result;}
    CalcOperation calcOperation() const {return _calcOperation;}

    void setFirstValue(int const & newValue);
    void setSecondValue(int const & newValue);
    void setResult(double const & newValue);
    void setCalcOperation(CalcOperation const & newOperation);

    Q_INVOKABLE void firstSpinboxActivated(int const & value);
    Q_INVOKABLE void secondSpinboxActivated(int const & value);
    Q_INVOKABLE void comboboxActivated(int const & id);

signals:
    void firstValueChanged();
    void secondValueChanged();
    void resultChanged();
    void calcOperationChanged();

private:
    int _firstValue;
    int _secondValue;
    double _result;
    CalcOperation _calcOperation;

    void calculate();
};

#endif // CALCULATORBACKEND_H
