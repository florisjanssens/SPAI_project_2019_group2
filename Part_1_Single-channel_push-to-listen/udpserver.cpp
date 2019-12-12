#include "udpserver.h"

// Constructor which creates the server on a passed address and port.
UDPServer::UDPServer(const QHostAddress &address, const quint16 port):
    serverAddress{address}, serverPort{port}, udpSocket{std::make_shared<QUdpSocket>(this)}
{}

// Constructor which creates the server on the host address and a passed port.
// Use if you just want to create the server on your host ip (192.168.0.7 for example).
// Doesn't always work in case you have multiple hosts ip (you could have multiple adapters)
UDPServer::UDPServer(const quint16 port):
    serverAddress{UDPServer::getLocalAddress()}, serverPort{port}, udpSocket{std::make_shared<QUdpSocket>(this)}
{}

// Destructor: closes the socket
UDPServer::~UDPServer()
{
    udpSocket->close();
}

// Static method to get the host IP (like 192.168.0.7)
QHostAddress UDPServer::getLocalAddress()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             return address;
    }
    return QHostAddress::LocalHost;
}

// Start the server on the address and port defined in the constructors.
// Binds the address to the port.
// Returns 1 on success
// Returns 0 on failure
// Also emits an error signal on failure
bool UDPServer::startServer()
{
    // QAbstractSocket::readyRead is emitted when a new datagram is available for reading
    // connect, connect this signal to the readyRead slot of this class.
    // So when the QAbstractSocket::readyRead signal is emitted (data available),
    // the UDPServer::readyRead() method is called.
    connect(udpSocket.get(), &QAbstractSocket::readyRead,
            this, &UDPServer::readyRead);

    bool success = udpSocket->bind(serverAddress, serverPort);

    // If bind failed, emit error signal with error message
    if(!success) emit errorMessage(udpSocket->errorString());

    return success;
}

// Reads a datagram and emits the datagramReceived signal when
// the datagram was read succesfully. The datagramReceived signal can then be used
// in some other class or window, etc. which can then get the peer info and data from the datagram.
void UDPServer::readyRead()
{
    const qint64 bytesToRead = udpSocket->pendingDatagramSize();

    if (bytesToRead < 0) // readyRead() was called but there is no/incorrect datagram.
    {
        emit warningMessage(tr("False read notification received"));
        return;
    }

    QByteArray datagram(int(bytesToRead), Qt::Uninitialized);
    QHostAddress peerAddress;
    quint16 peerPort = 0;

    // Read the datagram and get the data, size, peerAddress and peerPort of the sender
    const qint64 bytesRead = udpSocket->readDatagram(datagram.data(),
                                                     datagram.size(),
                                                     &peerAddress,
                                                     &peerPort);

    if (bytesRead < 0) // Datagram was incorrect
    {
        emit warningMessage(tr("Failed to read datagram: ") + QString::number(bytesRead) + "   " + udpSocket->errorString());
        return;
    } else if (bytesRead == 0) { // Datagram was empty
        emit warningMessage(tr("Empty datagram received"));
        return;
    }

    if (peerAddress.isNull() || !peerPort) // Could not get info of sender
    {
        emit warningMessage(tr("Failed to extract peer info"));
        return;
    }

    datagram.resize(int(bytesRead));

    // Emit the datagramReceived signal on successful  read.
    emit datagramReceived(peerAddress, peerPort, datagram);
    //qDebug() << "Datagram received";
}

// Some getters
quint16 UDPServer::getServerPort() const
{
    return serverPort;
}

std::vector<QHostAddress> UDPServer::getLocalAddresses()
{
    std::vector<QHostAddress> addresses;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             addresses.push_back(address);
    }

    return addresses;
}

QHostAddress UDPServer::getServerAddress() const
{
    return serverAddress;
}
