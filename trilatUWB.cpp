#include "TrilatUWB.h"
//#include <QThread>
#include "protocoluwb.h"

TrilatUWB:: TrilatUWB(QObject *parent)
{

//    connect(&timeCalc, &QTimer::timeout, this, &TrilatUWB::distanceCalc);
    connect(&timer, &QTimer::timeout, this, &TrilatUWB::updateMap);
    timer.start(1000);
//    timeCalc.start(10);
}

float  TrilatUWB::saturation(float input, double &prevOutput, float max, float min)
{

    if (input>= max)  return prevOutput;
    else if (input <=min) return prevOutput;
    else
    {
        prevOutput = input;
        return input;
    }
}

float TrilatUWB::filter(float input, double &output, int &i, int j)
{

        if (i <5)
        {
//            prevOutput = input;
         filt_mas[i][j] = input;
//         qDebug()<<"вывод" <<  filt_mas[i][j];
          i++;
//                     filt_mas[i] = 0;
        }
        else i =0;
        if (i==5)   i = 0;
        output = (filt_mas[0][j] + filt_mas[1][j] + filt_mas[2][j] + filt_mas[3][j] + filt_mas[4][j])/5;
//       qDebug()<<"вывод output" <<   output;
//       qDebug()<<"вывод i" <<   i;
        return output;

}

void TrilatUWB::integrate(double &input, double &output, double &prevOutput, double dt) {

    output = (prevOutput + dt*input/1)/(1+dt/1);
//    qDebug() << output;
    prevOutput = output;
}

void TrilatUWB::distanceCalc(RecDataUWB msg)
{
    dt = timeRegulator.elapsed()*0.001;//реальный временной шаг цикла
    timeRegulator.start();
    //сейчас будут написаны переменные, которые в дальнейшем должны быть внесены в Данину программу
//    float a01 = 2.3533;
    float a01 =2.6252;
    float a02 = 2.363312;
    float a03 = 2.2705;
    float a12 = 2.3328;
    float a13 = 2.2946;
    float a23 = 2.2574;

//    float const01 = 32918;
    float const01 = 32901;
    float const02 = 32951;
    float const03 = 32915;
    float const12 = 32922;
    float const13 = 32907;
    float const23 = 32955;

//    qDebug() << dataSend.time01;
//    X[71][0] = dataSend.time01;
//    X[53][0] = saturation(X[71][0], X[52][0], X[52][1], 40000, 30000);
//    integrate(X[53][0], X[61][0], X[61][1], dt);
//    X[102][0] = saturation(dataSend.time01, X[102][1], 40000, 20000);
//    integrate(X[102][0], X[101][0], X[101][1], dt);


    dist.distance01 = ((msg.time01-const01)/a01 + (msg.time10-const01)/a01)/2;
    dist.distance02 = ((msg.time02-const02)/a02 + (msg.time20-const02)/a02)/2;
    dist.distance03 = ((msg.time03-const03)/a03 + (msg.time30-const03)/a03)/2;
    dist.distance12 = ((msg.time12-const12)/a12 + (msg.time21-const12)/a12)/2;
    dist.distance13 = ((msg.time13-const13)/a13 + (msg.time31-const13)/a13)/2;
    dist.distance23 = ((msg.time23-const01)/a23 + (msg.time32-const23)/a23)/2;

    X[1][0] = saturation(dist.distance01, X[11][1], 1400, 0);
    X[2][0] = saturation(dist.distance02, X[12][1], 1400, 0);
    X[3][0] = saturation(dist.distance03, X[13][1], 1400, 0);
    X[4][0] = saturation(dist.distance12, X[14][1], 1400, 0);
    X[5][0] = saturation(dist.distance13, X[15][1], 1400, 0);
    X[6][0] = saturation(dist.distance23, X[16][1], 1400, 0);

    integrate(X[1][0], X[41][0], X[41][1], dt);
    integrate(X[2][0], X[42][0], X[42][1], dt);
    integrate(X[3][0], X[43][0], X[43][1], dt);
    integrate(X[4][0], X[44][0], X[44][1], dt);
    integrate(X[5][0], X[45][0], X[45][1], dt);
    integrate(X[6][0], X[46][0], X[46][1], dt);

    distAp.distance01 = X[41][0];
    distAp.distance02 = X[42][0];
    distAp.distance03 = X[43][0];
    distAp.distance12 = X[44][0];
    distAp.distance13 = X[45][0];
    distAp.distance23 = X[46][0];

//    qDebug() << "distance01: " << distAp.distance01;
//    qDebug() << "distance02: " << distAp.distance02;
//    qDebug() << "distance03: " << distAp.distance03;
//    qDebug() << "distance12: " << distAp.distance12;
//    qDebug() << "distance13: " << distAp.distance13;
//    qDebug() << "distance23: " << distAp.distance23;

    trilater();

}

void TrilatUWB::trilater()
{
 // примем,что x1y1 (0, 100); x2y2(0,0), x3y3(100,0)
    x1 = 0;
    x2 = 0;
    x3 = 130;
    y1 = 60;
    y2 = 0;
    y3 = 0;
    z1 = z2= z3 = 0;
    //следующие две формулы будут использоваться, если 1(0, y1, 0), 2(0, 0, 0), 3(x1, 0, 0)
    float y0 = (y1*y1 + (distAp.distance02)*(distAp.distance02) - distAp.distance01*distAp.distance01)/(2*y1);
    float x0 = (x3*x3+distAp.distance02*distAp.distance02-distAp.distance03*distAp.distance03)/(2*x3);
    //следующие две формулы будут использоваться, если 1(x1, y1, 0), 2(0, 0, 0), 3(x1, 0, 0)
//    float x0 = (x3*x3+dist.distance02*dist.distance02-dist.distance03*dist.distance03)/(2*x3);
//    float y0 = (y1*y1 + x1*x1 + (dist.distance02)*(dist.distance02) - dist.distance01*dist.distance01)/(2*y1) - x0*x1/y1;
//    float z0 = sqrt(dist.distance02*dist.distance02-x0*x0-y0*y0);
//    qDebug() << x0;
//    qDebug() << y0;
    X[21][0] = x0;
    X[22][0] = y0;
    integrate(X[21][0], X[51][0], X[51][1], dt);
    integrate(X[22][0], X[52][0], X[52][1], dt);
}



void TrilatUWB::updateMap()
{
    emit renewR1(X[41][0]);
    emit renewR2(X[42][0]);
    emit renewR3(X[43][0]);
    emit renewCurrentCoords(X[51][0],X[52][0]);
}
