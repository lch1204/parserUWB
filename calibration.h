#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "protocoluwb.h"
#include "cs_rov.h"

class Calibration: public QObject
{
    Q_OBJECT
public:
    Calibration( UWB::ProtocolUWB *prot, QObject *parent= nullptr, uint16_t size=100);
    uint T_act[3][3];
    uint T_meas[3][3];
    uint iteretion = 0;
    uint first = 513; //задержка анетнны от которой отталкиваемся

    uint32_t* distance01;
    uint32_t* distance02;
    uint32_t* distance03;
    uint32_t* distance12;
    uint32_t* distance13;
    uint32_t* distance10;
    uint32_t* distance20;
    uint32_t* distance30;
    uint32_t* distance21;
    uint32_t* distance31;




    uint32_t* delay_data_01; //массив данных возможных задержек
    uint32_t* delay_data_02;
    uint32_t* delay_data_03;
    uint32_t* delay_data_12;
    uint32_t* delay_data_13;
    uint32_t* delay_data_10;
    uint32_t* delay_data_20;
    uint32_t* delay_data_30;
    uint32_t* delay_data_21;
    uint32_t* delay_data_31;

    int*  delta01;
    int*  delta02;
    int*  delta03;
    int*  delta12;
    int*  delta13;
    int*  delta10;
    int*  delta20;
    int*  delta30;
    int*  delta21;
    int*  delta31;

    void integrate(uint32_t &input, double &output, double &prevOutput, double dt);

    float dt =0;
    QTime timeRegulator;

    uint16_t delay_size; //размер массива
    uint16_t variation = 800;
    double a = 2.3;
    UWB::ProtocolUWB * protUWB= nullptr;
public slots:
    void newCalibration(uint16_t size);
    void analysisCalib (uint16_t size = 100);


};

#endif // CALIBRATION_H
