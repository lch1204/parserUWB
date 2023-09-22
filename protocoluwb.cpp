#include "protocoluwb.h"
#include <math.h>

protocolUWB::protocolUWB(const QString& portName,
                         const qint32 baud,
                         const QSerialPort::DataBits data,
                         const QSerialPort::StopBits stop,
                         const QSerialPort::Parity parity,
                         const QSerialPort::FlowControl flow,
                         QObject* parent) :
    mPortName(portName),
    mBaud(baud),
    mDataBits(data),
    mStopBits(stop),
    mParity(parity),
    mFlow(flow),
    mTimer(this) {
    connect(&mTimer, &QTimer::timeout, this, &protocolUWB::tick_UWB);
    connect(&timer, &QTimer::timeout, this, &protocolUWB::updateMap);
    timer.start(1000);

    //алгоритм обращения к каждому биту
//    for (int i = 15; i>=0; i --)
//    {
//        if (prov & (1<<i))
//            qDebug() << "1";
//        else
//            qDebug() << "0";
//    }
}

void protocolUWB::start()
{
    if (protUWB->isOpen()) {
        qDebug() << "Duplicate open request";
        return;
    }
    protUWB->setPortName(mPortName);
    protUWB->setBaudRate(mBaud);
    protUWB->setDataBits(mDataBits);
    protUWB->setStopBits(mStopBits);
    protUWB->setParity(mParity);
    protUWB->setFlowControl(mFlow);
    if (!protUWB->open(QIODevice::ReadWrite)) {
        qDebug() << "Error connecting to MCU" << protUWB->errorString();
        return;
    }
//    mTimer.start(REQUEST_TIME);
    mTimer.start(1000);
    qDebug() << "Port opened";
    isConnected = true;
//    sendData();

    emit started();
    return;
}

void protocolUWB::tick_UWB()
{
//    sendData();
//    qDebug() << "все норм";
   if( sendData())
   {
       mTimer.stop();
       qDebug() << "все норм";
       connect(protUWB, &QSerialPort::readyRead, this, &protocolUWB::recData);
   }
}

void protocolUWB::recData()
{
//    qDebug() << "все норм1";
    m_buffer.append(protUWB->readAll());
//    qDebug() << "все норм2";
    parseBuffer();
}

void protocolUWB::parseBuffer()
{
    if (m_buffer.size() <= 2 ) //проверка, что размер буфера больше, чем размер сообщения
    // исправить 4 на правдивое значение
    {
        return;
    }
    QByteArray header((char*) &(headerUWB),sizeof(HeaderUWB));
    int index = m_buffer.indexOf(header);
    if (index == -1) {
        // Не найдено сообщение
//        qDebug() << "нет сообщения в буфере ";
        return;
    }
    if ( m_buffer.size() <= index + 58 ) {// заменить 55
        return;
    }
    if (correctChecksum(m_buffer.mid(index, 58))) {
        auto tmp = m_buffer.mid(index, 58);
        msg = reinterpret_cast<RecDataUWB*>(m_buffer.data()+index);
//        qDebug() << "time23:        " << msg->time23;
//        printBuffer();
        dataSend = *msg;
        distanceCalc();
        m_buffer.remove(0, index+57);
//        m_buffer.clear();
    }
    else {
        m_buffer.remove(0, index+1);
    }

    return;
}

void protocolUWB::printBuffer()
{
    qDebug() << "agents_number:     " << msg->headerUWB.agents_number;
    qDebug() << "self_index:        " << msg->headerUWB.self_index;
    qDebug() << "error_code:        " << msg->error_code;
    qDebug() << "iteration:         " << msg->iteration;
    qDebug() << "message_length:    " << msg->message_length;
    qDebug() << "connection_field:  " << msg->connection_field;
    qDebug() << "time01:        " << msg->time01;
    qDebug() << "time02:        " << msg->time02;
    qDebug() << "time03:        " << msg->time03;
    qDebug() << "time10:        " << msg->time10;
    qDebug() << "time12:        " << msg->time12;
    qDebug() << "time13:        " << msg->time13;
    qDebug() << "time20:        " << msg->time20;
    qDebug() << "time21:        " << msg->time21;
    qDebug() << "time23:        " << msg->time23;
    qDebug() << "time30:        " << msg->time30;
    qDebug() << "time31:        " << msg->time31;
    qDebug() << "time32:        " << msg->time32;
    qDebug() << "crc:               " << msg->crc;
        for (int i = 7; i>=0; i --)
        {
            if (msg->connection_field & (1<<i))
                qDebug() << "1";
            else
                qDebug() << "0";
        }
}

bool protocolUWB::sendData() {
    bool ret  = true;
    QByteArray packet;
// вся посылка не правильная, исправлю когда дима скажет нормальную посылку
    packet.append(dataSend.headerUWB.agents_number);
    packet.append(dataSend.headerUWB.self_index);
    qint16  crc = calculateCRC(packet);
    qint8 crc_low = crc & 0xff;
    qint8 crc_high = (crc >> 8);
    packet.append(crc_low);
    packet.append(crc_high);
    qint64 bytesWritten = protUWB->write(packet);
//    qint64 bytesWritten = protUWB->read(packet);
    if ((bytesWritten != packet.size())) {
        qDebug() << protUWB->errorString();
        ret = false;
    }
    packet.clear();
    return ret;
}

unsigned short protocolUWB::calculateCRC(QByteArray array) {

    int len = array.size();
    quint16 wcrc = 0xFFFF; // preset 16 position crc register , The initial values are all 1
    quint8 temp;// Define intermediate variables
    int i = 0, j = 0; // Define count
    for (i = 0; i < len; i++) { // Cycle through each data

        temp = array.at(i);
        wcrc ^= temp;
        for (j = 0; j < 8; j++) {

            // Judge whether what is moved to the right is 1, If it is 1 XOR with polynomials .
            if (wcrc & 0x0001) {

                wcrc >>= 1; // First move the data one bit to the right
                wcrc ^= 0xA001; // XOR with the polynomial above
            } else // If not 1, Then directly remove
                wcrc >>= 1; // Direct removal
        }
    }
    temp = wcrc; //crc Value
    return wcrc;
}

bool protocolUWB::correctChecksum (QByteArray const &ba) {
    qint16  crc = calculateCRC(ba.mid(0, 56));
    qint8 crc_low = crc & 0xff;
    qint8 crc_high = (crc >> 8);
    if ((crc_low == ba[56]) & (crc_high == ba[57])) {
        return true;
    }
    return false;
}

void protocolUWB::stop() {
    QMutexLocker lock(&mGuard);
    if (protUWB && protUWB->isOpen())
        protUWB->close();
    emit finished();
}



protocolUWB::~protocolUWB() {
    stop();
}

//--------------------------------------------------------------------------------
//окончание парсинга посылки, далее идет разбор посылки
//--------------------------------------------------------------------------------




void prikol ()
{

}
