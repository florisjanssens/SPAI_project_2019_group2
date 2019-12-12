#include "udpclient.h"

// Constructor which sets the serverAddress and port and initialize socket
// Use if you don't want to constantly pass the serverAddress and port while calling sendDatagram.
UDPClient::UDPClient(const QHostAddress &serverAddress, const quint16 serverPort):
    serverAddress{serverAddress}, serverPort{serverPort}, maxDatagramSize{512}, udpSocket{std::make_shared<QUdpSocket>(this)}
{}

// Constructor which just initializes the socket
// Use if you just want to pass the serverAddress and port later while calling sendDatagram.
UDPClient::UDPClient():
    serverAddress{QHostAddress::Null}, serverPort{0}, maxDatagramSize{512}, udpSocket{std::make_shared<QUdpSocket>(this)}
{
    //qDebug() << "In constructor: serverPort = " << serverPort << " while maxDatagramSize = " << maxDatagramSize;
}

// Destructor, closes the socket
UDPClient::~UDPClient()
{
    udpSocket->close();
}

// Send a datagram using a serverAddress and port which were set beforehand
// Useful to not pass the address and port everytime you want to send something.
bool UDPClient::sendDatagram(const QByteArray &datagram)
{
    return sendDatagram(datagram, serverAddress, serverPort);
}

// Send a datagram to the address and port passed to the method.
// You can change the address and port constantly because the client doesn't
// need to bind.
// Also emits warning signals when the datagram was empty or failed to send.
bool UDPClient::sendDatagram(const QByteArray &datagram, const QHostAddress &destServer, quint16 destPort)
{
    if (datagram.isEmpty())
    {
        //qDebug() << "Datagram is empty.";
        emit warningMessage("Tried to send empty datagram");
        return false;
    }

    // maxDatagramSize is 512 bytes here. Look in udpclient.h for reasoning.
    if (datagram.size() > maxDatagramSize)
    {
        //qDebug() << "Datagram size: " << datagram.size() << " while maxDatagramSize = " << maxDatagramSize;
        emit warningMessage("Datagram exceeds maxDatagramSize of " + QString::number(maxDatagramSize) + " bytes");
        return false;
    }

    qint64 ret = udpSocket->writeDatagram(datagram, destServer, destPort);
    if (ret < 0)
    {
        //qDebug() << "Ret: " << ret;
        emit warningMessage(tr("Failed to send datagram: ") + " " + udpSocket->errorString());
        return false;
    }

    //qDebug() << "sendDatagram OK";
    return true;
}

// Some getters and setters
QHostAddress UDPClient::getServerAddress() const
{
    return serverAddress;
}

void UDPClient::setServerAddress(const QHostAddress &value)
{
    serverAddress = value;
}

int UDPClient::getMaxDatagramSize() const
{
    return maxDatagramSize;
}

quint16 UDPClient::getServerPort() const
{
    return serverPort;
}
