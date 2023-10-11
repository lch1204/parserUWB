#ifndef TrilatUWB_H
#define TrilatUWB_H

#include "protocoluwb.h"
#include <math.h>
#include "protocol_bort_AUV/pc_protocol.h"
namespace UWB {


#pragma pack(push,1)
struct Distance2
{
    float distance01 = 0;
    float distance02 = 0;
    float distance03 = 0;
    float distance12 = 0;
    float distance13 = 0;
    float distance23 = 0;

};

#pragma pack(pop)

class TrilatUWB: public QObject
{
    Q_OBJECT
public:
   TrilatUWB(QObject *parent, ControlSystem::PC_Protocol *proto);
   QTimer timer;
   QTimer timeCalc;
   float filt_mas[10][6];
   Distance2 dist;
   Distance2 distAp;

protected:
   float dt =0;
   QTime timeRegulator;
   int x1, x2, x3, y1, y2, y3, z1, z2, z3;
   ControlSystem::PC_Protocol *auvProtocol = nullptr;
   float saturation(float input, double &prevOutput, float max, float min);
   float filter(float input, double &output, int &i, int j);
   void integrate(double &input, double &output, double &prevOutput, double dt);

   void trilater();
    ProtocolUWB *pUWB = nullptr;


public slots:
   void updateMap();
   void distanceCalc(RecDataUWB msg);// разбираем что пришло в посылке с датчика

signals:
    void renewR1(double r);
    void renewR2(double r);
    void renewR3(double r);
    void renewCurrentCoords(double _x, double _y);

};
} //namespace UWB
#endif // TrilatUWB_H
