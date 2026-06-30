#include "textfinder.h"
#include "./ui_textfinder.h"

TextFinder::TextFinder(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextFinder)
{
    ui->setupUi(this);
}

TextFinder::~TextFinder()
{
    delete ui;
}

void TextFinder::on_findButton_clicked()
{
    ui->label->setText("meow");
}

void TextFinder::loadTextFile()
{

}

