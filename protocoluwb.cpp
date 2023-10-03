#include "protocoluwb.h"
#include <math.h>
#include "QDataStream"


ProtocolUWB::ProtocolUWB(const QString& portName,
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
    connect(&mTimer, &QTimer::timeout, this, &ProtocolUWB::tick_UWB);
    connect(&timerCalib, &QTimer::timeout, this, &ProtocolUWB::calibData);
    timerCalib.start(5000);


    //алгоритм обращения к каждому биту
//    for (int i = 15; i>=0; i --)
//    {
//        if (prov & (1<<i))
//            qDebug() << "1";
//        else
//            qDebug() << "0";
//    }
}

void ProtocolUWB::start()
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
    emit started();
    return;
}

void ProtocolUWB::tick_UWB()
{
//    sendData();
   if( sendData())
   {
       mTimer.stop();
       qDebug() << "все норм";
       connect(protUWB, &QSerialPort::readyRead, this, &ProtocolUWB::recData);
   }
}

void ProtocolUWB::calibData()
{
    bool ret  = true;
    calibrationUWB.delay0=calibrationUWB.delay1=calibrationUWB.delay2=calibrationUWB.delay3=calibrationUWB.delay0+1;
    QByteArray packet;
    packet.append(calibrationUWB.message_type);
    packet.append(calibrationUWB.message_length);
    quint8 delay0_low = calibrationUWB.delay0 & 0xff;
    quint8 delay0_high = (calibrationUWB.delay0 >> 8);
    packet.append(delay0_low);
    packet.append(delay0_high);
    quint8 delay1_low = calibrationUWB.delay1 & 0xff;
    quint8 delay1_high = (calibrationUWB.delay1 >> 8);
    packet.append(delay1_low);
    packet.append(delay1_high);
    quint8 delay2_low = calibrationUWB.delay2 & 0xff;
    quint8 delay2_high = (calibrationUWB.delay2 >> 8);
    packet.append(delay2_low);
    packet.append(delay2_high);
    quint8 delay3_low = calibrationUWB.delay3 & 0xff;
    quint8 delay3_high = (calibrationUWB.delay3 >> 8);
    packet.append(delay3_low);
    packet.append(delay3_high);
    qint16  crc = calculateCRC(packet);
    qint8 crc_low = crc & 0xff;
    qint8 crc_high = (crc >> 8);
    packet.append(crc_low);
    packet.append(crc_high);
    qint64 bytesWritten = protUWB->write(packet);
    if ((bytesWritten != packet.size())) {
        qDebug() << protUWB->errorString();
        ret = false;
    }
    qDebug() << "отправлено: число байт: " <<bytesWritten<<" калибровочный коэффициент"<< calibrationUWB.delay0;
    packet.clear();
}

void ProtocolUWB::recData()
{
    m_buffer.append(protUWB->readAll());
    parseBuffer();
}

void ProtocolUWB::parseBuffer()
{
    if (m_buffer.size() <= 2 ) //проверка, что размер буфера больше, чем размер сообщения
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
        qDebug() << "маленький буфер";
        return;
    }
    if (correctChecksum(m_buffer.mid(index, 58))) {
        auto tmp = m_buffer.mid(index, 58);
        QDataStream stream(&tmp, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream >> msg.headerUWB.agents_number;
        stream >> msg.headerUWB.self_index;
        stream >> msg.error_code;
        stream >> msg.iteration;
        stream >> msg.message_length;
        stream >> msg.connection_field;
        stream >> msg.time01;
        stream >> msg.time02;
        stream >> msg.time03;
        stream >> msg.time10;
        stream >> msg.time12;
        stream >> msg.time13;
        stream >> msg.time20;
        stream >> msg.time21;
        stream >> msg.time23;
        stream >> msg.time30;
        stream >> msg.time31;
        stream >> msg.time32;
        stream >> msg.crc;
//        printBuffer();
        emit renewMapMsg(msg);
        emit renew(100);
        if (msg.error_code == 0)
        {
        emit renewMSG(msg);
        }
        m_buffer.remove(0, index+57);
    }
    else {
        qDebug() << "фигню отправил";
        m_buffer.remove(0, index+1);
         }
    return;
}

void ProtocolUWB::printBuffer()
{
    qDebug() << "agents_number:     " << msg.headerUWB.agents_number;
    qDebug() << "self_index:        " << msg.headerUWB.self_index;
    qDebug() << "error_code:        " << msg.error_code;
    qDebug() << "iteration:         " << msg.iteration;
    qDebug() << "message_length:    " << msg.message_length;
    qDebug() << "connection_field:  " << msg.connection_field;
    qDebug() << "time01:            " << msg.time01;
    qDebug() << "time02:            " << msg.time02;
    qDebug() << "time03:            " << msg.time03;
    qDebug() << "time10:            " << msg.time10;
    qDebug() << "time12:            " << msg.time12;
    qDebug() << "time13:            " << msg.time13;
    qDebug() << "time20:            " << msg.time20;
    qDebug() << "time21:            " << msg.time21;
    qDebug() << "time23:            " << msg.time23;
    qDebug() << "time30:            " << msg.time30;
    qDebug() << "time31:            " << msg.time31;
    qDebug() << "time32:            " << msg.time32;
    qDebug() << "crc:               " << msg.crc;
        for (int i = 7; i>=0; i --)
        {
            if (msg.connection_field & (1<<i))
                qDebug() << "1";
            else
                qDebug() << "0";
        }
}

bool ProtocolUWB::sendData() {
    bool ret  = true;
    QByteArray packet;
    packet.append(dataSend.message_type);
    packet.append(dataSend.message_length);
    packet.append(dataSend.headerUWB.agents_number);
    packet.append(dataSend.headerUWB.self_index);
    packet.append(dataSend.null);
    packet.append(dataSend.null);
    packet.append(dataSend.null);
    packet.append(dataSend.null);
    packet.append(dataSend.null);
    packet.append(dataSend.null);
    qint16  crc = calculateCRC(packet);
    qint8 crc_low = crc & 0xff;
    qint8 crc_high = (crc >> 8);
    packet.append(crc_low);
    packet.append(crc_high);
    qint64 bytesWritten = protUWB->write(packet);
    if ((bytesWritten != packet.size())) {
        qDebug() << protUWB->errorString();
        ret = false;
    }
    qDebug()<<"Отправлено через send data "<< bytesWritten << "байт";
    packet.clear();
    return ret;
}



unsigned short ProtocolUWB::calculateCRC(QByteArray array) {
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

bool ProtocolUWB::correctChecksum (QByteArray const &ba) {
    qint16  crc = calculateCRC(ba.mid(0, 56));
    qint8 crc_low = crc & 0xff;
    qint8 crc_high = (crc >> 8);
    if ((crc_low == ba[56]) & (crc_high == ba[57])) {
        return true;
    }
    qDebug() << "плохая crc";
    return false;
}

void ProtocolUWB::stop() {
    QMutexLocker lock(&mGuard);
    if (protUWB && protUWB->isOpen())
        protUWB->close();
    emit finished();
}

ProtocolUWB::~ProtocolUWB() {
    stop();
}
