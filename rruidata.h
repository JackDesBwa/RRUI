#ifndef RRUIDATA_H
#define RRUIDATA_H

#include <QObject>
#include <QQmlPropertyMap>

class RRUIData : public QQmlPropertyMap
{
    Q_OBJECT

public:
    explicit RRUIData(QObject *parent = 0);
};

#endif // RRUIDATA_H
