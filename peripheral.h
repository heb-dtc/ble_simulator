#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <QObject>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include "messenger.h"

class Peripheral : public QObject
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
    explicit Peripheral(const QString &name, QObject *parent = nullptr);
    ~Peripheral();
    void advertise();

signals:

public slots:
    void onConnected();
    void onDisconnected();
    void onStateChanged(QLowEnergyController::ControllerState state);
    void onError(QLowEnergyController::Error error);
    void onMessageReceived(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);
    void onMessageRead(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);
    void onMessageWritten(const QLowEnergyCharacteristic& characteritic, const QByteArray& data);
    void onDescriptorRead(const QLowEnergyDescriptor descriptor, const QByteArray &data);
    void onDescriptorWritten(const QLowEnergyDescriptor descriptor, const QByteArray &data);

private:
    QString name;
    Messenger *messenger;

    QLowEnergyController *lowEnergyController;
    QLowEnergyService *messagingService;
    QLowEnergyService *genericAttributeService;
    QLowEnergyService *genericAccessService;

    void sendMessage(const QByteArray &data);
};

#endif // PERIPHERAL_H
