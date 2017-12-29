#include "peripheral.h"
#include "akolytmessenger.h"

static constexpr const char* MESSAGING_SERVICE_UUID = "00010000-f619-54a4-95e5-072a926cc46f";
static constexpr const char* WRITE_CHARACTERISTIC_UUID = "00020000-f619-54a4-95e5-072a926cc46f";
static constexpr const char* NOTIFY_CHARACTERISTIC_UUID = "00020001-f619-54a4-95e5-072a926cc46f";

Peripheral::Peripheral(const QString &name, QObject *parent) : QObject(parent)
{
    this->name = name;
    this->messenger = new AkolytMessenger(this);
    this->lowEnergyController = QLowEnergyController::createPeripheral(parent);
    this->genericAccessService = this->lowEnergyController->addService(buildGenericAccessServiceData());
    this->genericAttributeService = this->lowEnergyController->addService(buildGenericAttributeServiceData());
    this->messagingService = this->lowEnergyController->addService(buildMessagingServiceData());

    QObject::connect(lowEnergyController, &QLowEnergyController::connected, this, &Peripheral::onConnected);
    QObject::connect(lowEnergyController, &QLowEnergyController::disconnected, this, &Peripheral::onDisconnected);
    QObject::connect(lowEnergyController, &QLowEnergyController::stateChanged, this, &Peripheral::onStateChanged);
    
    QObject::connect(messagingService, &QLowEnergyService::characteristicChanged, this, &Peripheral::onMessageReceived);
    QObject::connect(messagingService, &QLowEnergyService::characteristicRead, this, &Peripheral::onMessageRead);
    QObject::connect(messagingService, &QLowEnergyService::characteristicWritten, this, &Peripheral::onMessageWritten);
    QObject::connect(messagingService, &QLowEnergyService::descriptorRead, this, &Peripheral::onDescriptorRead);
    QObject::connect(messagingService, &QLowEnergyService::descriptorWritten, this, &Peripheral::onDescriptorWritten);
}

Peripheral::~Peripheral()
{
    delete this->lowEnergyController;
    delete this->genericAccessService;
    delete this->genericAttributeService;
    delete this->messagingService;
}

void Peripheral::advertise()
{
    QLowEnergyAdvertisingData advertisingData = buildAdvertisingData();
    this->lowEnergyController->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData, advertisingData);

    qDebug() << "Advertising for " << this->name << " has started...";
}

void Peripheral::onConnected()
{
    qDebug() << "controller connected";

    QLowEnergyCharacteristic notifyCharacteristic = this->messagingService->characteristic(QUuid(NOTIFY_CHARACTERISTIC_UUID));
    QLowEnergyDescriptor descriptor = notifyCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    this->messagingService->writeDescriptor(descriptor, QByteArray::fromHex("0200"));
}

void Peripheral::onDisconnected()
{
    qDebug() << "controller disconnected";
    advertise();
}

void Peripheral::onStateChanged(QLowEnergyController::ControllerState state)
{
    qDebug() << "controller state changed " << state;
}

void Peripheral::onError(QLowEnergyController::Error error)
{
    qDebug() << "controller error " << error;
}

void Peripheral::onMessageReceived(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    const auto sendCallback = [&] (const QByteArray & message) {
        sendMessage(message);
    };
    messenger->onMessageReceived(data, sendCallback); 
}

void Peripheral::onMessageWritten(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    qDebug() << "onMessageWritten";
}

void Peripheral::onMessageRead(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    qDebug() << "on message read";
}

void Peripheral::onDescriptorRead(const QLowEnergyDescriptor descriptor, const QByteArray &data)
{
    qDebug() << "on descriptor read";
}

void Peripheral::onDescriptorWritten(const QLowEnergyDescriptor descriptor, const QByteArray &data)
{
    qDebug() << "onDescriptorWritten";
}

void Peripheral::sendMessage(const QByteArray &data)
{
    qDebug() << "trying to send: " << data;

    QLowEnergyCharacteristic notifyCharacteristic = this->messagingService->characteristic(QUuid(NOTIFY_CHARACTERISTIC_UUID));
    this->messagingService->writeCharacteristic(notifyCharacteristic, data, QLowEnergyService::WriteWithResponse);
}

QLowEnergyAdvertisingData Peripheral::buildAdvertisingData()
{
    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName(this->name);
    advertisingData.setServices(getGattServices());
    return advertisingData;
}

QList<QBluetoothUuid> Peripheral::getGattServices()
{
    QList<QBluetoothUuid> gattServices;
    gattServices.append(QBluetoothUuid::GenericAccess);
    gattServices.append(QBluetoothUuid::GenericAttribute);
    gattServices.append(QBluetoothUuid::DeviceInformation);
    return gattServices;
}

QLowEnergyServiceData Peripheral::buildGenericAttributeServiceData()
{
    QLowEnergyServiceData genericAttributeService;

    genericAttributeService.setType(QLowEnergyServiceData::ServiceTypePrimary);
    genericAttributeService.setUuid(QBluetoothUuid::GenericAttribute);

    QLowEnergyCharacteristicData serviceChangedCharacteristic;
    serviceChangedCharacteristic.setUuid(QBluetoothUuid::ServiceChanged);
    serviceChangedCharacteristic.setProperties(QLowEnergyCharacteristic::PropertyType::Read | QLowEnergyCharacteristic::PropertyType::Indicate);
    serviceChangedCharacteristic.setValue(QByteArray(2, 0));
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));
    serviceChangedCharacteristic.addDescriptor(clientConfig);

    genericAttributeService.addCharacteristic(serviceChangedCharacteristic);

    return genericAttributeService;
}

QLowEnergyServiceData Peripheral::buildGenericAccessServiceData()
{
    QLowEnergyServiceData genericAccessService;

    QLowEnergyCharacteristicData deviceNameCharacteristic;
    deviceNameCharacteristic.setUuid(QBluetoothUuid::DeviceName);
    deviceNameCharacteristic.setProperties(QLowEnergyCharacteristic::PropertyType::Read);

    QLowEnergyCharacteristicData appearenceCharacteristic;
    appearenceCharacteristic.setUuid(QBluetoothUuid::Appearance);
    appearenceCharacteristic.setProperties(QLowEnergyCharacteristic::PropertyType::Read);

    QLowEnergyCharacteristicData peripheralPreferredConnectionParameters;
    peripheralPreferredConnectionParameters.setUuid(QBluetoothUuid::PeripheralPreferredConnectionParameters);
    peripheralPreferredConnectionParameters.setProperties(QLowEnergyCharacteristic::PropertyType::Read);

    genericAccessService.setType(QLowEnergyServiceData::ServiceTypePrimary);
    genericAccessService.setUuid(QBluetoothUuid::GenericAccess);
    genericAccessService.addCharacteristic(deviceNameCharacteristic);
    genericAccessService.addCharacteristic(appearenceCharacteristic);

    return genericAccessService;
}

QLowEnergyServiceData Peripheral::buildMessagingServiceData()
{
    QLowEnergyServiceData serviceData;

    QUuid serviceUuid(MESSAGING_SERVICE_UUID);
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(serviceUuid);

    serviceData.addCharacteristic(buildNotifyCharacteristic());
    serviceData.addCharacteristic(buildWriteCharacteristic());

    return serviceData;
}

QLowEnergyCharacteristicData Peripheral::buildNotifyCharacteristic()
{
    QLowEnergyCharacteristicData notifyCharacteristicData;

    QUuid notifyCharacteristicUuid(NOTIFY_CHARACTERISTIC_UUID);
    notifyCharacteristicData.setUuid(notifyCharacteristicUuid);
    notifyCharacteristicData.setValue(QByteArray(2, 0));
    notifyCharacteristicData.setProperties(QLowEnergyCharacteristic::Indicate);

    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));
    notifyCharacteristicData.addDescriptor(clientConfig);

    return notifyCharacteristicData;
}

QLowEnergyCharacteristicData Peripheral::buildWriteCharacteristic()
{
    QLowEnergyCharacteristicData writeCharacteristicData;

    QUuid writeCharacteristicUuid(WRITE_CHARACTERISTIC_UUID);
    writeCharacteristicData.setUuid(writeCharacteristicUuid);
    writeCharacteristicData.setProperties(QLowEnergyCharacteristic::Write);

    return writeCharacteristicData;
}
