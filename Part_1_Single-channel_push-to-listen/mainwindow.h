#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "audioin.h"
#include "audioout.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_connectButton_clicked();

    void on_speakButton_clicked();

    void showNotificationMessage(const QString &message);

    void cancel();

    // Check if this message comes from the device which we want to listen to
    void checkAndPlayMessage(const QHostAddress &peerAddress,
                     const quint16 peerPort,
                     QByteArray &datagram);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_comboBox_currentIndexChanged(const QString &ip);

    void on_serverBackButton_clicked();

    void on_clientBackButton_clicked();

    void on_listenButton_toggled(bool checked);

signals:
    void readyToPlay(const QByteArray * sound);
    void readyToRecord(std::shared_ptr<UDPClient> client, QHostAddress address, quint16 port);

private:
    Ui::MainWindow *ui;
    bool listening;
    bool speaking;

    std::shared_ptr<UDPServer> server = nullptr;
    std::shared_ptr<UDPClient> client = nullptr;


    std::shared_ptr<AudioIN> audIN = nullptr;
    std::shared_ptr<AudioOUT> audioOut = nullptr;

    quint16 port;

};
#endif // MAINWINDOW_H
