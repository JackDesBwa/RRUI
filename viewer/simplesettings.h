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

#ifndef SIMPLESETTINGS_H
#define SIMPLESETTINGS_H

#include <QObject>
#include <QSettings>

class SimpleSettings : public QObject
{
    Q_OBJECT

public:
    SimpleSettings();

public slots:
    void set_float(QString key, float value);
    float get_float(QString key);

    void set_string(QString key, QString value);
    QString get_string(QString key);

private:
    QSettings settings;
};

#endif // SIMPLESETTINGS_H
