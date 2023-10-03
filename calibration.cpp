#include "calibration.h"
//#include <initializer_list>
#include <random>
#include "protocoluwb.h"
#include "math.h"
//#include <iostream>


Calibration::Calibration( ProtocolUWB *prot, QObject *parent, uint16_t size)
{
    protUWB = prot;
    float frequency = 63.8976;
    float c = 29.9792458;
    //заполнение матрицы EDM_act
    T_act[0][0] = 0; //диаганаль
    T_act[1][1] = 0; //диаганаль
    T_act[2][2] = 0; //диаганаль
    T_act[0][1] = 100*frequency/c; // d01
    T_act[0][2] = 100*frequency/c; // d02
    T_act[1][0] = 100*frequency/c; // d10
    T_act[1][0] = 100*frequency/c; // d12
    T_act[2][0] = 100*frequency/c; // d20
    T_act[2][1] = 100*frequency/c; // d21
    delay_data_01 = new uint32_t[size];
    delay_data_02 = new uint32_t[size];
    delay_data_12 = new uint32_t[size];
    delay_data_10 = new uint32_t[size];
    delay_data_20 = new uint32_t[size];
    delay_data_21 = new uint32_t[size];

    distance01 = new uint32_t[size];
    distance02 = new uint32_t[size];
    distance12 = new uint32_t[size];
    distance10 = new uint32_t[size];
    distance20 = new uint32_t[size];
    distance21 = new uint32_t[size];

    delta01  = new int [size];
    delta02  = new int [size];
    delta12  = new int [size];
    delta10  = new int [size];
    delta20  = new int [size];
    delta21  = new int [size];



//    for (int i = 0; i < size; i++) {
//       delay_data_01[i] = 0;// выводим элементы массива
//     }
//    newCalibration(size);
    }

void Calibration:: newCalibration(uint16_t size)
{
    qDebug() << "зашел";

    for (iteretion=0; iteretion<=100;iteretion++)
    {
        if (iteretion == 1)
        {
            std::random_device rd;   // non-deterministic generator
            std::mt19937 gen(rd());  // to seed mersenne twister.
            std::uniform_int_distribution<> dist((32700),(33100)); // distribute results between 1 and 6 inclusive.

                for (int i = 0; i < size; ++i) {
                      delay_data_01[i] = dist(gen);
                      delay_data_02[i] = dist(gen);
                      delay_data_12[i] = dist(gen);
                      delay_data_10[i] = dist(gen);
                      delay_data_20[i] = dist(gen);
                      delay_data_21[i] = dist(gen);
//                    qDebug() <<i << ": "<< delay_data_01[i];// pass the generator to the distribution.
                }
        analysisCalib(size);
        }
        if (iteretion > 1)
        {
            variation = variation - 8;
//               qDebug()<<" "<< "+: " << delay_data_01[0]; // выводим элементы массива
               std::random_device rd;   // non-deterministic generator
               std::mt19937 gen(rd());
               std::uniform_int_distribution<> distNew(delay_data_01[0]-200, delay_data_01[0]+200);
                for (int i = size*1/4; i<=size; i++)
                {
                    delay_data_01[i] = distNew(gen);
                    delay_data_02[i] = distNew(gen);
                    delay_data_12[i] = distNew(gen);
                    delay_data_10[i] = distNew(gen);
                    delay_data_20[i] = distNew(gen);
                    delay_data_21[i] = distNew(gen);
//                    qDebug()<< i<<" "<< "+: " << delay_data_01[i];
                }



        }
    }
    qDebug() << "delay_data_01: " << delay_data_01[0];
    qDebug() << "delay_data_02: " << delay_data_02[0];
    qDebug() << "delay_data_12: " << delay_data_12[0];
    qDebug() << "delay_data_10: " << delay_data_10[0];
    qDebug() << "delay_data_20: " << delay_data_20[0];
    qDebug() << "delay_data_21: " << delay_data_21[0];
    return;
}

void Calibration::analysisCalib(uint16_t size)
{
    for (int i = 0; i < size; i++)
    {
        distance01[i] = ((protUWB->msg.time01-delay_data_01[i])/a);
        distance02[i] = ((protUWB->msg.time02-delay_data_02[i])/a);
        distance12[i] = ((protUWB->msg.time12-delay_data_12[i])/a);
        distance10[i] = ((protUWB->msg.time10-delay_data_10[i])/a);
        distance20[i] = ((protUWB->msg.time20-delay_data_20[i])/a);
        distance21[i] = ((protUWB->msg.time21-delay_data_21[i])/a);

       delta01[i] = (100 - distance01[i]);
       delta02[i] = (100 - distance02[i]);
       delta12[i] = (100 - distance12[i]);
       delta10[i] = (100 - distance10[i]);
       delta20[i] = (100 - distance20[i]);
       delta21[i] = (100 - distance21[i]);

       delta01[i] = abs(delta01[i]);
       delta02[i] = abs(delta02[i]);
       delta12[i] = abs(delta12[i]);
       delta10[i] = abs(delta10[i]);
       delta20[i] = abs(delta20[i]);
       delta21[i] = abs(delta21[i]);
    }
    //сортируем много сортируем
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta01[j] > delta01[j + 1]) {
          int b =  delta01[j]; // создали дополнительную переменную
          int c = delay_data_01[j];
          delta01[j] = delta01[j+1];
          delay_data_01[j] = delay_data_01[j + 1]; // меняем местами
          delta01[j + 1] = b; // значения элементов
          delay_data_01[j + 1] = c;
        }
      }
    }
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta02[j] > delta02[j + 1]) {
          int b =  delta02[j]; // создали дополнительную переменную
          int c = delay_data_02[j];
          delta02[j] = delta02[j+1];
          delay_data_02[j] = delay_data_02[j + 1]; // меняем местами
          delta02[j + 1] = b; // значения элементов
          delay_data_02[j + 1] = c;
        }
      }
    }
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta12[j] > delta12[j + 1]) {
          int b =  delta12[j]; // создали дополнительную переменную
          int c = delay_data_12[j];
          delta12[j] = delta12[j+1];
          delay_data_12[j] = delay_data_12[j + 1]; // меняем местами
          delta12[j + 1] = b; // значения элементов
          delay_data_12[j + 1] = c;
        }
      }
    }
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta10[j] > delta10[j + 1]) {
          int b =  delta10[j]; // создали дополнительную переменную
          int c = delay_data_10[j];
          delta10[j] = delta10[j+1];
          delay_data_10[j] = delay_data_10[j + 1]; // меняем местами
          delta10[j + 1] = b; // значения элементов
          delay_data_10[j + 1] = c;
        }
      }
    }
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta20[j] > delta20[j + 1]) {
          int b =  delta20[j]; // создали дополнительную переменную
          int c = delay_data_20[j];
          delta20[j] = delta20[j+1];
          delay_data_20[j] = delay_data_20[j + 1]; // меняем местами
          delta20[j + 1] = b; // значения элементов
          delay_data_20[j + 1] = c;
        }
      }
    }
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (delta21[j] > delta21[j + 1]) {
          int b =  delta21[j]; // создали дополнительную переменную
          int c = delay_data_21[j];
          delta21[j] = delta21[j+1];
          delay_data_21[j] = delay_data_21[j + 1]; // меняем местами
          delta21[j + 1] = b; // значения элементов
          delay_data_21[j + 1] = c;
        }
      }
    }
}
