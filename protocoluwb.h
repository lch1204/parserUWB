#ifndef ProtocolUWB_H
#define ProtocolUWB_H

#include <QObject>
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

class ProtocolUWB: public QObject
{
    Q_OBJECT
public:
    RecDataUWB msg;
    DataUWB dataSend;
    RecDataUWB dataTril;
    Distance dist;
    Distance integ;
    explicit ProtocolUWB(const QString& portName,
                const qint32 baud,
                const QSerialPort::DataBits data = QSerialPort::Data8,
                const QSerialPort::StopBits stop = QSerialPort::OneStop,
                const QSerialPort::Parity parity = QSerialPort::NoParity,
                const QSerialPort::FlowControl flow = QSerialPort::NoFlowControl,
                QObject* parent = nullptr);
  void settings(QObject *parent = nullptr);
//  virtual ~ProtocolUWB();
  DataUWB dataUWB; // структура вывода данных
  HeaderUWB headerUWB;
  int i [6];

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

protected:
    QTimer timer;

    unsigned short calculateCRC(QByteArray array);
    QByteArray m_buffer;
    void parseBuffer();
    void printBuffer();
//    TrilatUWB *tUWB = nullptr;

signals:
    void started();
    void finished();

    void renewMSG(RecDataUWB msg);
    void renewMapMsg(RecDataUWB msg);

};

#endif // ProtocolUWB_H
