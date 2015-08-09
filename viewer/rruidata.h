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

#ifndef RRUIDATA_H
#define RRUIDATA_H

#include <QObject>
#include <QTcpSocket>
#include <QQmlPropertyMap>

class RRUIData : public QQmlPropertyMap
{
    Q_OBJECT
    Q_PROPERTY(enum states connected_state READ get_connected_state NOTIFY connected_state_changed)
    Q_PROPERTY(bool is_connected READ get_isconnected NOTIFY connected_state_changed)
    Q_ENUMS(states)

public:
    enum states {
        STATE_OFF,
        STATE_CONNECTING,
        STATE_CONNECTED,
        STATE_DISCONNECTED,
        STATE_ERROR
    };

    explicit RRUIData(QObject *parent = 0);

    enum states get_connected_state() {
        return state;
    }

    bool get_isconnected() {
        return state == STATE_CONNECTED;
    }

signals:
    void connected_state_changed();

public slots:
    void connect(QString address, int port);
    void disconnect();

    void set_messages_file(QString messages_conf);
    void set_messages_string(QString messages_conf);

    void rr_get(QString rrname);
    void rr_set(QString rrname, float value);
    void rr_force(QString rrname, float value);
    void rr_release(QString rrname);
    void rr_reload(QString rrname);
    void rr_save(QString rrname, float value);
    void rr_export(QString rrname, int period);
    void rr_unexport(QString rrname);

private slots:
    void on_connected();
    void on_connection_error(QAbstractSocket::SocketError);
    void on_ready_to_read();

private:
    QTcpSocket s;
    enum states state;

    bool internal_update_flag;
    QMap <quint32, QString> map;

    QVariant updateValue(const QString &key, const QVariant &input);
    void internal_update(const QString &key, const float value);
    bool send_data(const unsigned char * const data, const int len);
    void send(QString rrname, float value, char action);
    void parse_line(QString line);
};

#endif // RRUIDATA_H
