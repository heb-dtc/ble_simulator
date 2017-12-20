#include "akolyt.h"

static constexpr const char* ADVERTISING_NAME = "Akolyt";

static constexpr const char* MESSAGING_SERVICE_UUID = "00010000-f619-54a4-95e5-072a926cc46f";
static constexpr const char* WRITE_CHARACTERISTIC_UUID = "00020000-f619-54a4-95e5-072a926cc46f";
static constexpr const char* NOTIFY_CHARACTERISTIC_UUID = "00020001-f619-54a4-95e5-072a926cc46f";

Akolyt::Akolyt(QObject *parent) : QObject(parent)
{
    this->lowEnergyController = QLowEnergyController::createPeripheral(parent);
    this->genericAccessService = this->lowEnergyController->addService(buildGenericAccessServiceData());
    this->genericAttributeService = this->lowEnergyController->addService(buildGenericAttributeServiceData());
    this->messagingService = this->lowEnergyController->addService(buildMessagingServiceData());

    QObject::connect(messagingService, &QLowEnergyService::characteristicChanged, this, &Akolyt::onMessageReceived);
    QObject::connect(messagingService, &QLowEnergyService::characteristicRead, this, &Akolyt::onReadRequest);
    QObject::connect(messagingService, &QLowEnergyService::characteristicWritten, this, &Akolyt::onCharacteristicWritten);
}

Akolyt::~Akolyt()
{
    delete this->lowEnergyController;
    delete this->genericAccessService;
    delete this->genericAttributeService;
    delete this->messagingService;
}

void Akolyt::advertise()
{
//    QLowEnergyCharacteristic notifyCharacteristic = this->messagingService->characteristic(QUuid(NOTIFY_CHARACTERISTIC_UUID));
//    QLowEnergyDescriptor d = notifyCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
//    this->messagingService->writeDescriptor(d, QByteArray::fromHex("0200"));

    QLowEnergyAdvertisingData advertisingData = buildAdvertisingData();
    this->lowEnergyController->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData, advertisingData);

    qDebug() << "Advertising for " << ADVERTISING_NAME << " has started...";
}

void Akolyt::onMessageReceived(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    qDebug() << "message received " << data;

    QLowEnergyCharacteristic notifyCharacteristic = this->messagingService->characteristic(QUuid(NOTIFY_CHARACTERISTIC_UUID));

    QByteArray value;
    value.append(char(0));
    value.append(char(1));

    qDebug() << "trying to send: " << value;

    QLowEnergyDescriptor descriptor = notifyCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    this->messagingService->writeDescriptor(descriptor, QByteArray::fromHex("0200"));

    this->messagingService->writeCharacteristic(notifyCharacteristic, value);
    //this->messagingService->writeDescriptor(descriptor, value);
}

void Akolyt::onReadRequest(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    qDebug() << "read request received" << data;
}

void Akolyt::onCharacteristicWritten(const QLowEnergyCharacteristic &characteritic, const QByteArray &data)
{
    qDebug() << "onCharacteristicWritten" << data;
}

QLowEnergyAdvertisingData Akolyt::buildAdvertisingData()
{
    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName(ADVERTISING_NAME);
    advertisingData.setServices(getGattServices());
    return advertisingData;
}

QList<QBluetoothUuid> Akolyt::getGattServices()
{
    QList<QBluetoothUuid> gattServices;
    gattServices.append(QBluetoothUuid::GenericAccess);
    gattServices.append(QBluetoothUuid::GenericAttribute);
    gattServices.append(QBluetoothUuid::DeviceInformation);
    return gattServices;
}

QLowEnergyServiceData Akolyt::buildGenericAttributeServiceData()
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

QLowEnergyServiceData Akolyt::buildGenericAccessServiceData()
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

QLowEnergyServiceData Akolyt::buildMessagingServiceData()
{
    QLowEnergyServiceData serviceData;

    QUuid serviceUuid(MESSAGING_SERVICE_UUID);
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(serviceUuid);

    serviceData.addCharacteristic(buildNotifyCharacteristic());
    serviceData.addCharacteristic(buildWriteCharacteristic());

    return serviceData;
}

QLowEnergyCharacteristicData Akolyt::buildNotifyCharacteristic()
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

QLowEnergyCharacteristicData Akolyt::buildWriteCharacteristic()
{
    QLowEnergyCharacteristicData writeCharacteristicData;

    QUuid writeCharacteristicUuid(WRITE_CHARACTERISTIC_UUID);
    writeCharacteristicData.setUuid(writeCharacteristicUuid);
    writeCharacteristicData.setProperties(QLowEnergyCharacteristic::Write);

    return writeCharacteristicData;
}
