#ifndef PROTOCOLUWB_H
#define PROTOCOLUWB_H

#include <QSerialPort>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QBitArray>
#include <QByteArray>
#include <iostream>
#include <QMutex>


//для использования kx-pult
extern double X[2000][2];
extern QVector<double> K;

#pragma pack(push,1)
//заглушка для заголовка послыки
struct HeaderUWB  {
    uint8_t agents_number = 4;
    uint8_t self_index = 0;

};

struct DataUWB {


    HeaderUWB headerUWB;
    uint16_t crc = 0;
  //  uint16_t agentIdent = 0;// идентификация подключенных агентов
    //посылка которую должен написать Дима
};

struct RecDataUWB
{
    HeaderUWB headerUWB;
    uint8_t error_code = 0x00;
    uint8_t iteration = 0x00;
    uint16_t message_length = 0;
    uint16_t connection_field = 0;
    uint32_t time01 = 0;
    uint32_t time02 = 0;
    uint32_t time03 = 0;
    uint32_t time10 = 0;
    uint32_t time12 = 0;
    uint32_t time13 = 0;
    uint32_t time20 = 0;
    uint32_t time21 = 0;
    uint32_t time23 = 0;
    uint32_t time30 = 0;
    uint32_t time31 = 0;
    uint32_t time32 = 0;
    uint16_t crc = 0;
};

struct Distance
{
    float distance01 = 0;
    float distance02 = 0;
    float distance03 = 0;
    float distance12 = 0;
    float distance13 = 0;
    float distance23 = 0;

};

#pragma pack(pop)

class protocolUWB: public QObject
{
    Q_OBJECT
public:
    RecDataUWB dataSend;
    Distance dist;
    Distance integ;
    protocolUWB(const QString& portName,
                const qint32 baud,
                const QSerialPort::DataBits data = QSerialPort::Data8,
                const QSerialPort::StopBits stop = QSerialPort::OneStop,
                const QSerialPort::Parity parity = QSerialPort::NoParity,
                const QSerialPort::FlowControl flow = QSerialPort::NoFlowControl,
                QObject* parent = nullptr);
  void settings(QObject *parent);
  virtual ~protocolUWB();
  DataUWB dataUWB; // структура вывода данных
  HeaderUWB headerUWB;
  int i [6];
   float filt_mas[10][6] ;

   bool correctChecksum (QByteArray const &ba);//это метод, который проверяет корректность чексуммы

private:
  QSerialPort* protUWB = new QSerialPort(this);
  QString mPortName;
  qint32 mBaud;
  QSerialPort::DataBits mDataBits;
  QSerialPort::StopBits mStopBits;
  QSerialPort::Parity mParity;
  QSerialPort::FlowControl mFlow;
  QTimer mTimer;
  bool isConnected = false;
  bool sendData();
  void recData();
  static constexpr int REQUEST_TIME = 100;
  mutable QMutex mGuard;

private slots:
    void tick_UWB();

public slots:
    virtual void start();
    virtual void stop();
    void updateMap();
signals:
    void renewR1(double r);
    void renewR2(double r);
    void renewR3(double r);
    void renewCurrentCoords(double _x, double _y);

protected:
    QTimer timer;
    void integrate(double &input, double &output, double &prevOutput, double dt);
    unsigned short calculateCRC(QByteArray array);
    QByteArray m_buffer;
    void parseBuffer();
    void printBuffer();
    float dt =0;
    void matrix();
    float A[4][4];
    float b[4];
    void prikol();
    float distanceCalc();// разбираем что пришло в посылке с датчика
    float saturation(float input, double &prevOutput, float max, float min);
    float filter(float input, double &output, int &i, int j);
    RecDataUWB* msg;
    QTime timeRegulator;
    void trilater();
    int x1, x2, x3, y1, y2, y3, z1, z2, z3;


signals:
    void started();
    void finished();

};

#endif // PROTOCOLUWB_H
