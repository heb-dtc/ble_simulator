#ifndef MESSENGER_H
#define MESSENGER_H

#include <functional>
#include <QtCore/qbytearray.h>

class Messenger
{

  public:
    virtual ~Messenger();

    void onMessageReceived(const QByteArray &, std::function<void(const QByteArray&)>);
    
    protected:
      Messenger();

private:
  virtual void read(const QByteArray &, std::function<void(const QByteArray&)>) = 0;

};

#endif //MESSENGER_H
