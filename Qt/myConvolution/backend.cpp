#include "backend.h"
#include <QDebug>   // для работы с консолью

Backend::Backend(QObject *parent)
    : QObject{parent}
{}

void Backend::onGenerationButtonClicked()
{
    qDebug() << "Кнопка генерации нагрузки нажата.";

    db.openConnection();
}
