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
#include <QUrl>
#include <QDebug>
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
        ret.b = (value < -0.0000001f || value > 0.0000001f);
        break;
    case 1:
        ret.i8 = static_cast<int8_t>(value);
        break;
    case 2:
        ret.u8 = static_cast<uint8_t>(value);
        break;
    case 3:
        ret.i16 = static_cast<int16_t>(value);
        break;
    case 4:
        ret.u16 = static_cast<uint16_t>(value);
        break;
    case 5:
        ret.i32 = static_cast<int32_t>(value);
        break;
    case 6:
        ret.u32 = static_cast<uint32_t>(value);
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
        // Hum, try as if messages_conf is a URL
        config.setFileName(QUrl(messages_conf).toLocalFile());
        config.open(QFile::OpenModeFlag::ReadOnly);
    }
    if (!config.isOpen()) {
        qDebug() << "[RRUIDATA] Configuration file" << messages_conf << "cannot be read:" << config.errorString();
        return;
    }

    while (!config.atEnd()) {
        parse_line(config.readLine());
    }
}

void RRUIData::set_messages_string(QString messages_conf) {
    map.clear();
    QTextStream config(&messages_conf);
    while (!config.atEnd()) {
        parse_line(config.readLine());
    }
}

void RRUIData::parse_line(QString line) {
    QString a, b, key, value;
    int kind, number;

    // Read one line
    line = line.trimmed();
    if (line.length() == 0) return;
    if (line[0] == '#') return;

    // Split at first blank
    int startpos = line.indexOf(QRegExp("[ \t]"));
    if (startpos != -1) {
        a = line.left(startpos);
        b = line.mid(startpos + 1);
    }

    // Split first part at first dot
    startpos = a.indexOf(".");
    if (startpos == -1) {
        qDebug() << QString("[RRUIDATA] Error, invalid line %1").arg(line);
        return;
    }
    kind = a.left(startpos).toInt();
    number = a.mid(startpos + 1).toInt();
    if (kind > 255 || kind < 0) {
        qDebug() << QString("[RRUIDATA] Error, impossible variable kind %1").arg(kind);
        return;
    } else if (number > 65535 || number < 0) {
        qDebug() << QString("[RRUIDATA] Error, impossible variable number %1").arg(number);
        return;
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
    quint32 keynr = static_cast<quint32>(kind) << 16 | static_cast<uint16_t>(number);
    auto it = map.find(keynr);
    if (it != map.end()) {
        qDebug() << QString("[RRUIDATA] Error, duplicate id %1.%2 (first one is %3)").arg(kind).arg(number).arg(it.value());
        return;
    }

    // Record values
    map.insert(keynr, key);
    if (kind == 0) {
        bool val = false;
        if (value.compare("on") == 0) val = true;
        if (value.compare("1") == 0) val = true;
        if (value.compare("yes") == 0) val = true;
        internal_update(key, float(val));

    } else if (kind == 1) {
        internal_update(key, static_cast<float>(static_cast<int8_t>(value.toInt())));

    } else if (kind == 2) {
        internal_update(key, static_cast<float>(static_cast<uint8_t>(value.toInt())));

    } else if (kind == 3) {
        internal_update(key, static_cast<float>(static_cast<int16_t>(value.toInt())));

    } else if (kind == 4) {
        internal_update(key, static_cast<float>(static_cast<uint16_t>(value.toInt())));

    } else if (kind == 5) {
        internal_update(key, static_cast<float>(static_cast<int32_t>(value.toInt())));

    } else if (kind == 6) {
        internal_update(key, static_cast<float>(static_cast<uint32_t>(value.toInt())));

    } else if (kind == 7) {
        float val = 0;
        val = value.toFloat();
        internal_update(key, val);
    }
}

void RRUIData::internal_update(const QString &key, const float value) {
    internal_update_flag = true;
    this->setProperty(key.toStdString().c_str(), value);
    internal_update_flag = false;
}

QVariant RRUIData::updateValue(const QString &key, const QVariant &input) {
    if (!internal_update_flag) {
        qWarning() << QString("Trying to modify %1 value to %2 from QML. Please use set or force functions").arg(key).arg(input.toString());
        return value(key);
    }
    return input;
}

bool RRUIData::send_data(const unsigned char * const data, const int len) {
    return s.write(reinterpret_cast<const char*>(data), len) == len;
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
        quint32 key = static_cast<quint32>(data[i + BYTE_KIND]) << 16 | *reinterpret_cast<uint16_t*>(&data[i + BYTE_NUMBER]);
        float value = 1;
        switch (data[i + BYTE_KIND]) {
        case 0:
        case 1:
            value = static_cast<float>(*reinterpret_cast<int8_t*>(&data[i + BYTE_VALUE]));
            break;
        case 2:
            value = static_cast<float>(*reinterpret_cast<uint8_t*>(&data[i + BYTE_VALUE]));
            break;
        case 3:
            value = static_cast<float>(*reinterpret_cast<int16_t*>(&data[i + BYTE_VALUE]));
            break;
        case 4:
            value = static_cast<float>(*reinterpret_cast<uint16_t*>(&data[i + BYTE_VALUE]));
            break;
        case 5:
            value = static_cast<float>(*reinterpret_cast<int32_t*>(&data[i + BYTE_VALUE]));
            break;
        case 6:
            value= static_cast<float>(*reinterpret_cast<uint32_t*>(&data[i + BYTE_VALUE]));
            break;
        case 7:
            value = *reinterpret_cast<float*>(&data[i + BYTE_VALUE]);
            break;
        default:
            value = 0;
        }
        internal_update(map[key], value);
        i += 8;
    }
}

void RRUIData::connect(QString address, quint16 port) {
    if (state != STATE_CONNECTED) {
        qDebug() << QString("Connect to %1:%2").arg(address).arg(port);
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
    quint8 kind = 0;
    quint16 number = 0;
    auto it = map.constBegin();
    for (; it != map.constEnd(); ++it) {
        if (it.value().compare(rrname) == 0) {
            kind = static_cast<quint8>(it.key() >> 16);
            number = static_cast<quint16>(it.key());
            break;
        }
    }
    if (it == map.constEnd()) {
        qWarning() << QString("%1 not found").arg(rrname);
        return;
    }

    char msg[8];
    msg[BYTE_KIND] = static_cast<char>(kind);
    msg[BYTE_ACTION] = action;
    *reinterpret_cast<quint16*>(&msg[BYTE_NUMBER]) = number;
    if (action == ACTION_EXPORT)
        *reinterpret_cast<quint32*>(&msg[BYTE_VALUE]) = static_cast<quint32>(value);
    else
        *reinterpret_cast<valuable*>(&msg[BYTE_VALUE]) = float_to_valuable(kind, value);

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
