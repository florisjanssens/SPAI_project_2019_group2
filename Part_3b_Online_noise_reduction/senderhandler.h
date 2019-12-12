#ifndef SENDERHANDLER_H
#define SENDERHANDLER_H

#include <QTimer>
#include <memory>
#include "sender.h"

class SenderHandler : public QObject
{
    Q_OBJECT
public:
    // Make SenderHandler which checks for client timeouts every senderTimeoutSecs
    SenderHandler(int senderTimeoutSecs);
    // Get the amount of known clients
    unsigned long getNumberOfClients() const;
    // Get a vector of all the known client addresses
    std::vector<QHostAddress> getClientAddresses() const;

    void clearClients();

public slots:
    // Check if a client is already known and add them to the list of known clients otherwise
    void checkClientMembership(const QHostAddress &peerAddress,
                               const quint16 peerPort,
                               QByteArray &datagram);

signals:
    // Signal emitted when a new known client is added
    void newClientNotification(const QHostAddress &peerAddress);
    // Signal emitted when a known client times out
    void timeoutClientNotification(const QHostAddress &peerAddress);

private slots:
    // Check if any client needs to be timed out
    void checkClientTimeouts();

private:
    int senderTimeoutSecs;
    std::vector<std::shared_ptr<Sender>> clientList;
    std::shared_ptr<QTimer> timer = nullptr;
};

#endif // SENDERHANDLER_H
