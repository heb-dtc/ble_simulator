#include "messenger.h"

Messenger::Messenger()
{
}

Messenger::~Messenger()
{
}

void Messenger::onMessageReceived(const QByteArray &data, std::function<void(const QByteArray&)> callback)
{
  read(data, callback);
};