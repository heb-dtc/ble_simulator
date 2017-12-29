#include <QDebug>
#include "messagesender.h"

MessageSender::MessageSender()
{
  this->messages = QList<QByteArray>();
}

void MessageSender::prepare(QList<QByteArray> messages, std::function<const void(QByteArray&)> sendCallback)
{
  qDebug() << "Preparing message sender for " << messages.size() << " messages";

  this->messages.clear();
  this->messages.append(messages);
  this->callback = sendCallback;
  this->lastMessageIndex = -1;
}

int MessageSender::getIndexOfLastMessage() const
{
  return this->lastMessageIndex;
}

void MessageSender::sendNext()
{
  this->lastMessageIndex++;
  qDebug() << "Sending message " << this->lastMessageIndex;

  QByteArray message = this->messages.at(this->lastMessageIndex);
  this->callback(message);
}

