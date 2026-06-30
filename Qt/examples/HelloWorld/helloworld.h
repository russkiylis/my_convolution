#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class HelloWorld;
}
QT_END_NAMESPACE

class HelloWorld : public QMainWindow
{
    Q_OBJECT

public:
    HelloWorld(QWidget *parent = nullptr);
    ~HelloWorld();

private slots:
    void on_hello_world_button_clicked(bool checked);

    void on_number_selector_1_valueChanged(int arg1);

    void on_number_selector_2_valueChanged(int arg1);

    void on_operation_selector_currentIndexChanged(int index);

private:
    Ui::HelloWorld *ui;

    int number_1 = 0, number_2 = 0;
    enum Calc_Operation
    {
        Add,
        Substract,
        Multiply,
        Divide
    };
    Calc_Operation calc_operation = Add;
    double calc_result;



    void calculate();
};
#endif // HELLOWORLD_H
