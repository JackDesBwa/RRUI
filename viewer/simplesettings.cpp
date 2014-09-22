/*
    This file is part of RRUI.

    RRUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RRUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RRUI.  If not, see <http://www.gnu.org/licenses/>.
*/

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
