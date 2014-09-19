#include "rruidata.h"

RRUIData::RRUIData(QObject *parent) : QQmlPropertyMap(this, parent)
{
    qDebug("RRUIData init");
}
