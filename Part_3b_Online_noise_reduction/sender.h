#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QNetworkInterface>
#include <QDateTime>

class Sender {
public:
    Sender(const QHostAddress &senderAddress);

    QHostAddress getAddress() const;

    QDateTime getLastActive() const;
    void updateActiveTime();

private:
    const QHostAddress address;
    QDateTime lastActive;
};

#endif // SENDER_H
