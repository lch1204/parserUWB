#ifndef PARSUWB_H
#define PARSUWB_H

#include "protocoluwb.h"
#include <QTime>
#include <QDebug>

class parsUWB : public QObject
{
    Q_OBJECT
public:
    parsUWB(QObject * parent = nullptr);
    void readDataUWB();
protected:
    protocolUWB *protocol ;
};
#endif // PARSUWB_H
