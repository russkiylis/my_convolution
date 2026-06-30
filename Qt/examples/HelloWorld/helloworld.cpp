#include "helloworld.h"
#include "./ui_helloworld.h"

#include <QDebug>

HelloWorld::HelloWorld(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HelloWorld)
{
    ui->setupUi(this);

    this->calculate();
}

HelloWorld::~HelloWorld()
{
    delete ui;
}

void HelloWorld::on_hello_world_button_clicked(bool checked)
{
    qDebug() << "Hello World button pressed. Fact check status: " << checked;

    if (checked)
    {
        ui->hello_world_line->setText("Hello World!");
    }
    else
    {
        ui->hello_world_line->setText("");
    }
}


void HelloWorld::on_number_selector_1_valueChanged(int arg1)
{
    this->number_1 = arg1;

    qDebug() << "First number changed. New number = " << this->number_1;

    this->calculate();
}


void HelloWorld::on_number_selector_2_valueChanged(int arg1)
{
    this->number_2 = arg1;

    qDebug() << "Second number changed. New number = "  << this->number_2;

    this->calculate();
}


void HelloWorld::on_operation_selector_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        this->calc_operation = Add;
        break;
    case 1:
        this->calc_operation = Substract;
        break;
    case 2:
        this->calc_operation = Multiply;
        break;
    case 3:
        this->calc_operation = Divide;
        break;
    }

    qDebug() << "Calculator operation changed. New operation: " << ui->operation_selector->currentText();

    this->calculate();
}

void HelloWorld::calculate()
{
    switch (this->calc_operation)
    {
    case Add:
        this->calc_result = this->number_1 + this->number_2;
        break;
    case Substract:
        this->calc_result = this->number_1 - this->number_2;
        break;
    case Multiply:
        this->calc_result = this->number_1 * this->number_2;
        break;
    case Divide:
        if (this->number_2 == 0)
            {
            ui->result_line->setText("NaN");
            qDebug() << "Unfortunately I can't divide by zero.";
            return;
            }
        this->calc_result = double(this->number_1) / double(this->number_2);
        break;
    }

    ui->result_line->setText(QString::number(this->calc_result));

    qDebug() << "Calculation successful. Result = " << this->calc_result;
}
