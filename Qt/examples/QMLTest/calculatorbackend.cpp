#include "calculatorbackend.h"
#include <QDebug>

CalculatorBackend::CalculatorBackend(QObject *parent)
    : QObject{parent},
    _firstValue(0),
    _secondValue(0),
    _result(0),
    _calcOperation(CalcOperation(0))
{}

void CalculatorBackend::setFirstValue(int const & newValue) {
    if (_firstValue == newValue)
        return;
    _firstValue = newValue;
    emit firstValueChanged();
}

void CalculatorBackend::setSecondValue(int const & newValue) {
    if (_secondValue == newValue)
        return;
    _secondValue = newValue;
    emit secondValueChanged();
}

void CalculatorBackend::setResult(double const & newValue) {
    if (_result == newValue)
        return;
    _result = newValue;

    qDebug() << "Result changed. Value: " << _result;

    emit resultChanged();
}

void CalculatorBackend::setCalcOperation(CalcOperation const & newOperation) {
    if (_calcOperation == newOperation)
        return;
    _calcOperation = newOperation;
    emit calcOperationChanged();
}


// These are triggered when a user changes something
void CalculatorBackend::firstSpinboxActivated(int const & value) {
    qDebug() << "First calculator spinbox activated. value: " << value;
    setFirstValue(value);
    calculate();
}

void CalculatorBackend::secondSpinboxActivated(int const & value) {
    qDebug() << "Second calculator spinbox activated. value: " << value;
    setSecondValue(value);
    calculate();
}

void CalculatorBackend::comboboxActivated(int const & id) {
    qDebug() << "Calculator combobox activated. index: " << id;
    setCalcOperation(CalcOperation(id));
    calculate();
}

void CalculatorBackend::calculate() {
    int const & a = _firstValue;
    int const & b = _secondValue;

    switch (_calcOperation){
    case add:
        setResult(a + b);
        break;
    case substract:
        setResult(a - b);
        break;
    case multiply:
        setResult(a * b);
        break;
    case divide:
        setResult(double(a) / b);
        break;
    }
}


