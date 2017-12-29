#include <QDebug>
#include <QThread>
#include <QTimer>
#include "akolytmessenger.h"

AkolytMessenger::AkolytMessenger(QObject *parent) : QObject(parent), Messenger()
{
}

AkolytMessenger::~AkolytMessenger() 
{
}

void AkolytMessenger::read(const QByteArray &data, std::function<void(const QByteArray&)> callback)
{
    qDebug() << "message received" << data;
    qDebug() << "message lenght is " << data.length();

    MessageType messageType = parseType(data.at(0));
    QByteArray value;

    switch (messageType) 
    {
        case MessageType::SET_TOKEN:
            qDebug() << "set token received";
            value.append(static_cast<char>(MessageType::SET_TOKEN));
            //sending success
            value.append(0x01);
            callback(value);
            break;
        case MessageType::GET_VERSION:
            qDebug() << "get version received";
            value.append(static_cast<char>(MessageType::GET_VERSION));
            //stm32 1.19.0
            value.append(0x01);
            value.append(0x13);
            //cypress 1.2.0
            value.append(0x01);
            value.append(0x02);
            value.append((char)0x00);
            //cypress hw version 2
            value.append(0x02);
            //akolyt hw version
            value.append(0x02);
            //BLE protocol version
            value.append(0x01);
            //GPRS protocol version
            value.append((char)0x00);
            //firmware revision
            value.append((char)0x00);            
            callback(value);
            break;
        case MessageType::GET_VARIANT:
            qDebug() << "get variant received";
            value.append(static_cast<char>(MessageType::GET_VARIANT));
            //name (size need to be 10 bytes!)
            value.append(QByteArray::fromStdString("debug"));
            value.append((char)0x00);
            value.append((char)0x00);
            value.append((char)0x00);
            value.append((char)0x00);
            value.append((char)0x00);
            //variant
            value.append(QByteArray::fromStdString("b4f96e39"));
            value.append((char)0x00);
            callback(value);
            break;
        case MessageType::GET_VEHICULE_SIGNATURE:
            qDebug() << "get vehicule signature received, sending car identification responses";
            sendCarSignature(callback);
            break;
        case MessageType::SET_USER:
            qDebug() << "set user received";
            value.append(static_cast<char>(MessageType::SET_USER));
            //sending success (1 = true, 0 = false)
            value.append(0x01);
            callback(value);
            break;
        case MessageType::CLIENT_SETTINGS:
            qDebug() << "client settings received but not implemented (yet?)";
            value.append(static_cast<char>(MessageType::REFUSED));
            value.append(static_cast<char>(MessageType::CLIENT_SETTINGS));
            callback(value);
            break;
        case MessageType::GET_STATUS:
            qDebug() << "get status received";
            value.append(static_cast<char>(MessageType::GET_STATUS));
            //sending device is plugged
            value.append(0x01);
            //sending device need vehicle signature
            value.append(0x01);
            callback(value);
            break;
        case MessageType::UNKNOWN:
        default:
            qDebug() << "can't handle message, unknown type";
            value.append(static_cast<char>(MessageType::REFUSED));
            value.append((char)0x00);
            callback(value);
            break;
    }
}

MessageType AkolytMessenger::parseType(const unsigned char type)
{
    qDebug() << "trying to parse type";

    if (type == 0x00) {
        return MessageType::SET_TOKEN;
    } else if (type == 0x01){
        return MessageType::GET_VERSION;
    } else if (type == 0x02) {
        return MessageType::GET_VEHICULE_SIGNATURE;
    } else if (type == 0x20) {
        return MessageType::GET_VARIANT;
    } else if (type == 0xD) {
        return MessageType::GET_STATUS;
    } else if (type == 0x13) {
        return MessageType::SET_USER;
    } else if (type == 0x39) {
        return MessageType::CLIENT_SETTINGS;
    } else {
        return MessageType::UNKNOWN;
    }
}

void AkolytMessenger::sendCarSignature(std::function<void(const QByteArray&)> callback) 
{   
    QTimer::singleShot(0, this, SLOT(sendNextMessage(0, callback)));
}

//Six messages are sent one after the other
void AkolytMessenger::sendNextMessage(int index, std::function<const void(QByteArray&)> callback)
{
    QByteArray value;

    if (index == 0) {
        //Send a GET_RETURN_CODE with a new vehicle value
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //Not sure why this value but IT WORKS!
        value.append(0x05);
        //set value to NEW_VEHICLE(2) (other values are ERROR(0), SAME_VEHICLE(1), DIFFERENT_VEHICLE(3))
        value.append(0x02);
        callback(value);
        QTimer::singleShot(200, this, SLOT(sendNextMessage(1, callback)));
    } else if (index == 1) {
        //Send A GET_VIN response with a valid VIN
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //GET_VIN
        value.append((char)0x00);
        //VIN
        value.append(QByteArray::fromStdString("1A1JC5444R7252367"));
        callback(value);
        QTimer::singleShot(200, this, SLOT(sendNextMessage(2, callback)));
    } else if (index == 2) {
        //Send A GET_PROTOCOL response with a protocol value
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //GET_PROTOCOL
        value.append(0x01);
        //PROTOCOL
        value.append(0x01);
        callback(value);
        QTimer::singleShot(200, this, SLOT(sendNextMessage(3, callback)));
    } else if (index == 3) {
        //Send A GET_VHID_1 response with the first part of the vehicle ID value
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //GET_VHID_1
        value.append(0x02);
        //Vehicle ID part 1
        value.append(QByteArray::fromStdString("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000"));
        callback(value);
        QTimer::singleShot(200, this, SLOT(sendNextMessage(4, callback)));
    } else if (index == 4) {
        //Send A GET_VHID_2 response with the second part of the vehicle ID value
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //GET_VHID_2
        value.append(0x03);
        //Vehicle ID part 2
        value.append(QByteArray::fromStdString("000000000000000000000000000000000000"));
        callback(value);
        QTimer::singleShot(200, this, SLOT(sendNextMessage(5, callback)));
    } else if (index == 5) {
        //Send A GET_VHID_3 response with the thirs part of the vehicle ID value
        value.append(static_cast<char>(MessageType::GET_VEHICULE_SIGNATURE));
        //GET_VHID_3
        value.append(0x04);
        //Vehicle ID part 3
        value.append(0xFF);
        value.append(0xFF);
        value.append(0xFF);
        value.append(0xFF);
        callback(value);
    }
}