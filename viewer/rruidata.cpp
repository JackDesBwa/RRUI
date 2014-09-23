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

#include <QFile>
#include <stdint.h>
#include "rruidata.h"

#define BYTE_KIND   0 ///< Kind byte position
#define BYTE_ACTION 1 ///< Action byte position
#define BYTE_NUMBER 2 ///< Number LSB byte position
#define BYTE_VALUE  4 ///< Value LSB byte position

#define ACTION_REPORT   0 ///< Report action ID
#define ACTION_GET      1 ///< Get action ID
#define ACTION_SET      2 ///< Set action ID
#define ACTION_FORCE    3 ///< Forc action ID
#define ACTION_RELEASE  4 ///< Release action ID
#define ACTION_RELOAD   5 ///< Reload action ID
#define ACTION_SAVE     6 ///< Save action ID
#define ACTION_EXPORT   7 ///< Export action ID
#define ACTION_UNEXPORT 8 ///< Unexport action ID

union valuable {
    bool b;
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    float f;
};

static valuable float_to_valuable(int kind, float value) {
    valuable ret;
    switch (kind) {
        case 0:
            ret.b = (value == 0);
        case 1:
            ret.i8 = (int8_t) value;
            break;
        case 2:
            ret.u8 = (uint8_t) value;
            break;
        case 3:
            ret.i16 = (int16_t) value;
            break;
        case 4:
            ret.u16 = (uint16_t) value;
            break;
        case 5:
            ret.i32 = (int32_t) value;
            break;
        case 6:
            ret.u32 = (uint32_t) value;
            break;
        case 7:
            ret.f = value;
            break;
        default:
            ret.i32 = 0;
    }
    return ret;
}

RRUIData::RRUIData(QObject *parent) : QQmlPropertyMap(this, parent), state(STATE_OFF) {
    qDebug("RRUIData init");
    QObject::connect(&s, SIGNAL(connected()), this, SLOT(on_connected()));
    QObject::connect(&s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_connection_error(QAbstractSocket::SocketError)));
    QObject::connect(&s, SIGNAL(readyRead()), this, SLOT(on_ready_to_read()));
}

void RRUIData::set_messages_file(QString messages_conf) {
    map.clear();

    if (messages_conf.isEmpty()) {
        qDebug("[RRUIDATA] Configuration filename is empty");
        return;
    }

    QFile config(messages_conf);
    config.open(QFile::OpenModeFlag::ReadOnly);
    if (!config.isOpen()) {
        qDebug() << "[RRUIDATA] Configuration file" << messages_conf << "cannot be read";
        return;
    }

    while (!config.atEnd()) {
        QString a, b, key, value;
        int kind, number;

        // Read one line
        QString line = config.readLine().trimmed();
        if (line.length() == 0) continue;
        if (line[0] == '#') continue;

        // Split at first blank
        int startpos = line.indexOf(QRegExp("[ \t]"));
        if (startpos != -1) {
            a = line.left(startpos);
            b = line.mid(startpos + 1);
        }

        // Split first part at first dot
        startpos = a.indexOf(".");
        if (startpos != -1) {
            kind = a.left(startpos).toInt();
            number = a.mid(startpos + 1).toInt();
            if (kind > 255 || kind < 0) {
                qDebug("[RRUIDATA] Error, impossible variable kind " + kind);
                continue;
            } else if (number > 65535 || number < 0) {
                qDebug("[RRUIDATA] Error, impossible variable number " + number);
                continue;
            }
        }

        // Split second part at optional equal sign
        startpos = b.indexOf("=");
        if (startpos != -1) {
            key = b.left(startpos);
            value = b.mid(startpos + 1);
            int pos2 = value.indexOf(QRegExp("[^ \t]"));
            if (pos2 != -1) value = value.mid(pos2);
            pos2 = key.lastIndexOf(QRegExp("[ \t]"));
            if (pos2 != -1) value = value.left(pos2+1);
        } else {
            key = b;
        }

        // Trim parameter name
        int pos2 = key.indexOf(QRegExp("[ \t]"));
        if (pos2 != -1) key = key.mid(pos2);
        pos2 = key.lastIndexOf(QRegExp("[^ \t]"));
        if (pos2 != -1) key = key.left(pos2+1);

        // Verify unicity of parameters
        auto it = map.find(kind << 16 | number);
        if (it != map.end()) {
            qDebug((QString("[RRUIDATA] Error, duplicate id ") + kind + '.' + number +
            " (first one is " + it.value() + ")").toStdString().c_str());
            continue;
        }

        // Record values
        map.insert(kind << 16 | number, key);
        if (kind == 0) {
            bool val = false;
            if (value.compare("on") == 0) val = true;
            if (value.compare("1") == 0) val = true;
            if (value.compare("yes") == 0) val = true;
            internal_update(key, float(val));

        } else if (kind == 1) {
            int8_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 2) {
            uint8_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 3) {
            int16_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 4) {
            uint16_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 5) {
            int32_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 6) {
            uint32_t val = 0;
            val = value.toInt();
            internal_update(key, float(val));

        } else if (kind == 7) {
            float val = 0;
            val = value.toFloat();
            internal_update(key, val);
        }
    }
}

void RRUIData::internal_update(const QString &key, const float value) {
    internal_update_flag = true;
    this->setProperty(key.toStdString().c_str(), value);
    internal_update_flag = false;
}

QVariant RRUIData::updateValue(const QString &key, const QVariant &input) {
    if (!internal_update_flag) {
        QString msg = "Trying to modify " + key + " value to " + input.toString() + " from QML. Please use set or force functions";
        qWarning(msg.toStdString().c_str());
        return value(key);
    }
    return input;
}

bool RRUIData::send_data(const unsigned char * const data, const int len) {
    return s.write((char*) data, len) == len;
}

void RRUIData::on_connected() {
    qDebug("Connected");
    state = STATE_CONNECTED;
    emit connected_state_changed();
}

void RRUIData::on_connection_error(QAbstractSocket::SocketError) {
    if (state == STATE_CONNECTED) {
        qDebug("Disconnected");
        state = STATE_DISCONNECTED;
    } else {
        qDebug("Connection error");
        state = STATE_ERROR;
    }
    s.close();
    emit connected_state_changed();
}

void RRUIData::on_ready_to_read() {
    char data[1024];
    int i = 0;
    qint64 readlen = s.read(data, sizeof(data));
    while (i + 7 < readlen) {
        quint32 key = ((quint32) data[i + BYTE_KIND]) << 16 | *((uint16_t*)&data[i + BYTE_NUMBER]);
        float value = 1;
        switch (data[i + BYTE_KIND]) {
            case 0:
            case 1:
                value = (float)(*(int8_t*)&data[i + BYTE_VALUE]);
                break;
            case 2:
                value = (float)(*(uint8_t*)&data[i + BYTE_VALUE]);
                break;
            case 3:
                value = (float)(*(int16_t*)&data[i + BYTE_VALUE]);
                break;
            case 4:
                value = (float)(*(uint16_t*)&data[i + BYTE_VALUE]);
                break;
            case 5:
                value = (float)(*(int32_t*)&data[i + BYTE_VALUE]);
                break;
            case 6:
                value= (float)(*(uint32_t*)&data[i + BYTE_VALUE]);
                break;
            case 7:
                value = *(float*)&data[i + BYTE_VALUE];
                break;
            default:
                value = 0;
        }
        internal_update(map[key], value);
        i += 8;
    }
}

void RRUIData::connect(QString address, int port) {
    if (state != STATE_CONNECTED) {
        QString msg = "Connect to " + address + ":" + QString::number(port);
        qDebug(msg.toStdString().c_str());
        s.connectToHost(address, port);
        state = STATE_CONNECTING;
        emit connected_state_changed();
    }
}

void RRUIData::disconnect() {
    s.close();
    state = STATE_DISCONNECTED;
    emit connected_state_changed();
}

void RRUIData::send(QString rrname, float value, char action) {
    quint8 kind;
    quint16 number;
    auto it = map.constBegin();
    for (; it != map.constEnd(); ++it) {
        if (it.value().compare(rrname) == 0) {
            kind = it.key() >> 16;
            number = it.key();
            break;
        }
    }
    if (it == map.constEnd()) {
        qWarning((rrname + " not found").toStdString().c_str());
        return;
    }

    char msg[8];
    msg[BYTE_KIND] = kind;
    msg[BYTE_ACTION] = action;
    *((quint16*)&msg[BYTE_NUMBER]) = number;
    if (action == ACTION_EXPORT)
        *((quint32*)&msg[BYTE_VALUE]) = (quint32) value;
    else
        *((valuable*)&msg[BYTE_VALUE]) = float_to_valuable(kind, value);

    s.write(msg, 8);
}

void RRUIData::rr_get(QString rrname) {
    send(rrname, 0, ACTION_GET);
}

void RRUIData::rr_set(QString rrname, float value) {
    send(rrname, value, ACTION_SET);
}

void RRUIData::rr_force(QString rrname, float value) {
    send(rrname, value, ACTION_FORCE);
}

void RRUIData::rr_release(QString rrname) {
    send(rrname, 0, ACTION_RELEASE);
}

void RRUIData::rr_reload(QString rrname) {
    send(rrname, 0, ACTION_RELOAD);
}

void RRUIData::rr_save(QString rrname, float value) {
    send(rrname, value, ACTION_SAVE);
}

void RRUIData::rr_export(QString rrname, int period) {
    send(rrname, period, ACTION_EXPORT);
}

void RRUIData::rr_unexport(QString rrname) {
    send(rrname, 0, ACTION_UNEXPORT);
}