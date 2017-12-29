#ifndef AKOLYT_MESSENGER
#define AKOLYT_MESSENGER

#include <QObject>
#include "messenger.h"

enum class MessageType : unsigned char
{
  SET_TOKEN = 0x00,
  GET_VERSION = 0x01,
  GET_VARIANT = 0x20,
  GET_VEHICULE_SIGNATURE = 0x02,
  GET_STATUS = 0xD,
  SET_USER = 0x13,
  CLIENT_SETTINGS = 0x39,
  REFUSED = 0xFF,
  UNKNOWN = 0xF1
};

class AkolytMessenger : public QObject, public Messenger
{
  Q_OBJECT
  
  public:
    AkolytMessenger(QObject *parent = nullptr);
    virtual ~AkolytMessenger();

  signals:

  public slots:
      void sendNextMessage(int index, std::function<const void(QByteArray&)>);

  private:
    virtual void read(const QByteArray &, std::function<void(const QByteArray&)>);
    MessageType parseType(const unsigned char);
    void sendCarSignature(std::function<void(const QByteArray&)>);
    
};

#endif //AKOLYT_MESSENGER