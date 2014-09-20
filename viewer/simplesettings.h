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
