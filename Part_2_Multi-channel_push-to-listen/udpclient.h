#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <memory>

class UDPClient : public QObject
{
    Q_OBJECT
public:
    // Look at the udpclient.cpp for info
    UDPClient(const QHostAddress &serverAddress, const quint16 serverPort);
    UDPClient();
    ~UDPClient();

    /* Don't send blocks bigger than 512 bytes!
     * While it is theoretically possible, packets bigger than 512 bytes will probably
     * get fragmented by the IP layer. You should leave a margin for what happens with the
     * packet while it travels over the network (can get encapsulated for example).
     * You also don't know the maximum transmission unit of every network this will be used on.
     * 512 bytes is the standard for what's still safe!
     */
    bool sendDatagram(const QByteArray &datagram);
    bool sendDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);

    QHostAddress getServerAddress() const;
    void setServerAddress(const QHostAddress &value);

    quint16 getServerPort() const;

    int getMaxDatagramSize() const;

signals:
    void warningMessage(const QString &message);

private:
    // serverAddress and serverPort are not const now, can be changed
    QHostAddress serverAddress;
    const quint16 serverPort;
    const int maxDatagramSize;
    std::shared_ptr<QUdpSocket> udpSocket = nullptr; // Don't worry about freeing memory
    // If you use a normal pointer, don't forget to call delete!
};

#endif // UDPCLIENT_H
