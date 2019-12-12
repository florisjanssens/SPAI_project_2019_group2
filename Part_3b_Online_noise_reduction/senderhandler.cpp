#include "senderhandler.h"

// Make SenderHandler which checks for client timeouts every senderTimeoutSecs
SenderHandler::SenderHandler(int senderTimeoutSecs):
    senderTimeoutSecs{senderTimeoutSecs}
{
    timer = std::make_shared<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &SenderHandler::checkClientTimeouts);
    timer->start(senderTimeoutSecs*1000);
}

// Check if any client needs to be timed out
void SenderHandler::checkClientTimeouts()
{
    auto sender = std::find_if(clientList.begin(), clientList.end(), [&](std::shared_ptr<Sender> s){
        return s->getLastActive().secsTo(QDateTime::currentDateTime()) >= senderTimeoutSecs;
    });
    if (sender != clientList.end()) {
        QHostAddress addressToRemove = sender->get()->getAddress();
        clientList.erase(sender);
        emit timeoutClientNotification(addressToRemove);
    }
}

// Check if a client is already known and add them to the list of known clients otherwise
void SenderHandler::checkClientMembership(const QHostAddress &peerAddress,
                                          const quint16 peerPort,
                                          QByteArray &datagram)
{
    auto sender = std::find_if(clientList.begin(), clientList.end(), [&](std::shared_ptr<Sender> s){
        return s->getAddress() == peerAddress;
    });

    if (sender == clientList.end()){
        std::shared_ptr<Sender> s = std::make_shared<Sender>(peerAddress);
        clientList.push_back(s);
        emit newClientNotification(peerAddress);
    } else {
        sender->get()->updateActiveTime();
    }

}

// Get the amount of known clients
unsigned long SenderHandler::getNumberOfClients() const
{
    return clientList.size();
}

// Get a vector of all the known client addresses
std::vector<QHostAddress> SenderHandler::getClientAddresses() const
{
    std::vector<QHostAddress> clientAddresses;
    for (auto &e : clientList)
    {
        clientAddresses.push_back(e->getAddress());
    }
    return clientAddresses;
}

void SenderHandler::clearClients()
{
    clientList.clear();
}
