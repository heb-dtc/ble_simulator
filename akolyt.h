#ifndef AKOLYT_H
#define AKOLYT_H

#include <QObject>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>

class Akolyt : public QObject
{
    Q_OBJECT

    QList<QBluetoothUuid> getGattServices();

    QLowEnergyAdvertisingData buildAdvertisingData();
    QLowEnergyCharacteristicData buildNotifyCharacteristic();
    QLowEnergyCharacteristicData buildWriteCharacteristic();

    QLowEnergyServiceData buildGenericAttributeServiceData();
    QLowEnergyServiceData buildGenericAccessServiceData();
    QLowEnergyServiceData buildMessagingServiceData();

public:
    explicit Akolyt(QObject *parent = nullptr);
    ~Akolyt();
    void advertise();

signals:

public slots:
    void onMessageReceived(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);
    void onReadRequest(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);
    void onCharacteristicWritten(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);

private:
    QLowEnergyController *lowEnergyController;
    QLowEnergyService *messagingService;
    QLowEnergyService *genericAttributeService;
    QLowEnergyService *genericAccessService;
};

#endif // AKOLYT_H
