#include <QApplication>
#include "protocoluwb.h"
#include "kx_pult/kx_protocol.h"
#include "kx_pult/qkx_coeffs.h"
#include "map/map.h"
#include "trilatUWB.h"
#include <QObject>

const QString ConfigFile = "protocols.conf";
const QString XI = "xi";
const QString KI = "ki";

double X[2000][2];

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map map;
//    TrilatUWB trUWB;
    map.show();

    Qkx_coeffs* kProtocol = new Qkx_coeffs(ConfigFile, KI);
    //передача X
    x_protocol* xProtocol = new x_protocol(ConfigFile, XI, X);

    TrilatUWB *trUWB = new TrilatUWB();
    QString port ="COM3";
    qint32 baudrate = 115200;
    ProtocolUWB* protUWB = new ProtocolUWB (port, baudrate);
    QThread uwbThread;
    protUWB->moveToThread(&uwbThread);
    QObject::connect(&uwbThread, &QThread::started, protUWB, &ProtocolUWB::start);
    uwbThread.start();


    QObject::connect(protUWB, &ProtocolUWB::renewMSG, trUWB, &TrilatUWB::distanceCalc, Qt::BlockingQueuedConnection);
    QObject::connect(protUWB, &ProtocolUWB::renewMapMsg, &map, &Map::printInf, Qt::BlockingQueuedConnection);
    QObject::connect(trUWB, &TrilatUWB::renewR1,&map, &Map::drawCircle1);
    QObject::connect(trUWB, &TrilatUWB::renewR2, &map, &Map::drawCircle2);
    QObject::connect(trUWB, &TrilatUWB::renewR3, &map, &Map::drawCircle3);
    QObject::connect(trUWB, &TrilatUWB::renewCurrentCoords, &map, &Map::drawCurrentCoords);

    return a.exec();
}
