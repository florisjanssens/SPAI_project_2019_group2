#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <memory>
#include <QThread>
#include <QMetaType>
#include <QPushButton>

#include "audioin.h"
#include "audioout.h"
#include "senderhandler.h"
#include "processor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread processorThread;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void convertDatagram(const QByteArray &datagram);

private slots:

    void on_connectButton_clicked();

    void on_speakButton_clicked();

    void showNotificationMessage(const QString &message);

    void cancel();

    // Add a sender to the listWidget
    void addClientItem(const QHostAddress &peerAddress);

    // Remove a sender from the listWidget
    void removeClient(const QHostAddress &peerAddress);

    // When a button of one of the items of the listWidget is clicked
    void clicked(bool checked, const QHostAddress &peerAddress, QString buttonName);

    // Check if this message comes from the device which we want to listen to
    void checkAndPlayMessage(const QHostAddress &peerAddress,
                     const quint16 peerPort,
                     QByteArray &datagram);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_comboBox_currentIndexChanged(const QString &ip);

    void on_serverBackButton_clicked();

    void on_clientBackButton_clicked();

signals:
    void receivedData(QByteArray data);
    void readyToRecord(std::shared_ptr<UDPClient> client, QHostAddress address, quint16 port);

private:
    Ui::MainWindow *ui;
    bool listening;
    bool speaking;

    std::shared_ptr<UDPServer> server = nullptr;
    std::shared_ptr<UDPClient> client = nullptr;

    std::shared_ptr<SenderHandler> senderHandler = nullptr;
    // Store address of the sender we want to listen to
    QHostAddress selectedSender = QHostAddress::Null;

    std::shared_ptr<AudioIN> audIN = nullptr;
    std::shared_ptr<AudioOUT> audioOut = nullptr;

    quint16 port;

    std::shared_ptr<Processor> processor = nullptr;
};
#endif // MAINWINDOW_H
