#include <QApplication>
#include "protocoluwb.h"
#include "parsuwb.h"
#include <QThread>
#include "kx_pult/kx_protocol.h"
#include "kx_pult/qkx_coeffs.h"
#include "map/map.h"

const QString ConfigFile = "protocols.conf";
const QString XI = "xi";
const QString KI = "ki";

double X[2000][2];

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map map;
    map.show();

    Qkx_coeffs* kProtocol = new Qkx_coeffs(ConfigFile, KI);
    //передача X
    x_protocol* xProtocol = new x_protocol(ConfigFile, XI, X);

    QString port ="COM3";
    qint32 baudrate = 115200;
    protocolUWB* protUWB = new protocolUWB (port, baudrate);

    QThread uwbThread;
    protUWB->moveToThread(&uwbThread);
    QObject::connect(&uwbThread, &QThread::started, protUWB, &protocolUWB::start);
    uwbThread.start();
    QObject::connect(protUWB, &protocolUWB::renewR1,&map, &Map::drawCircle1);
    QObject::connect(protUWB, &protocolUWB::renewR2, &map, &Map::drawCircle2);
    QObject::connect(protUWB, &protocolUWB::renewR3, &map, &Map::drawCircle3);
    QObject::connect(protUWB, &protocolUWB::renewCurrentCoords, &map, &Map::drawCurrentCoords);
    return a.exec();
}
