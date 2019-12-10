#include "sender.h"

Sender::Sender(const QHostAddress &senderAddress):
    address{senderAddress}, lastActive{QDateTime::currentDateTime()}
{}

QHostAddress Sender::getAddress() const
{
    return address;
}

QDateTime Sender::getLastActive() const
{
    return lastActive;
}

void Sender::updateActiveTime()
{
    lastActive = QDateTime::currentDateTime();
}
