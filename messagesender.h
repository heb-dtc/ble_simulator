#ifndef MESSAGE_SENDER
#define MESSAGE_SENDER

#include <functional>
#include <QtCore/qbytearray.h>
#include <QList>

class MessageSender 
{
  public:
    MessageSender();
    void prepare(QList<QByteArray> messages, std::function<const void(QByteArray&)> sendCallback);
    void sendNext();
    int getIndexOfLastMessage() const;

  private:
    QList<QByteArray> messages;
    std::function<const void(QByteArray&)> callback;
    int lastMessageIndex;

};

#endif //MESSAGE_SENDER