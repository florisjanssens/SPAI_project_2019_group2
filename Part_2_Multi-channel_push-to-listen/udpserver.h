#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <memory>

class UDPServer : public QObject
{
    Q_OBJECT

public:
    // Look at udpserver.cpp for info
    UDPServer(const QHostAddress &address, const quint16 port);
    UDPServer(const quint16 port);
    ~UDPServer();
    bool startServer();
    QHostAddress getServerAddress() const;
    quint16 getServerPort() const;
    static std::vector<QHostAddress> getLocalAddresses();

signals:
    void datagramReceived(const QHostAddress &peerAddress,
                          const quint16 peerPort,
                          QByteArray &datagram);
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);

private slots:
    void readyRead();

private:
    static QHostAddress getLocalAddress();
    const QHostAddress serverAddress;
    const quint16 serverPort;
    std::shared_ptr<QUdpSocket> udpSocket = nullptr;
};

#endif // UDPSERVER_H
