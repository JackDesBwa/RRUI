#include "simplesettings.h"
#include <QDebug>

SimpleSettings::SimpleSettings() : settings("rrui", "rrui")
{
}

void SimpleSettings::set_float(QString key, float value) {
    settings.setValue(key, value);
    settings.sync();
}

void SimpleSettings::set_string(QString key, QString value) {
    settings.setValue(key, value);
    settings.sync();
}

float SimpleSettings::get_float(QString key) {
    float fval = settings.value(key, 0).toFloat();
    return fval;
}

QString SimpleSettings::get_string(QString key) {
    QString sval = settings.value(key, "").toString();
    return sval;
}
