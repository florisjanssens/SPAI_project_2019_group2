#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), listening(false), speaking(false)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(2);

    port = 54545;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * This button's behaviour will depend if the device acts as a server or client.
 * In case of server, UDP socket will be created.
 * In case of client, UDP socket will be connected to.
 */
void MainWindow::on_connectButton_clicked()
{
    /*
     * First of all, necessary connections are made such that
     * the server/client act accordingly.
     */
    if (ui->serverButton->isChecked())
    {
        ui->stackedWidget->setCurrentIndex(0);
        audioOut = std::make_shared<AudioOUT>();

        std::vector<QHostAddress> addresses = UDPServer::getLocalAddresses();
        for (auto &e : addresses)
            ui->comboBox->addItem(e.toString() + ":" + QString::number(port));
    } else {
        audIN = std::make_shared<AudioIN>();
        // Once the user is ready to record (i.e. the speak button has been clicked)
        // we will start sending packets to the server device
        connect(this, &MainWindow::readyToRecord, audIN.get(), &AudioIN::sendAudio);

        // show a dialog in which server IP and port will be inputted in order to connect
        ui->stackedWidget->setCurrentIndex(3);
    }
}

/*
 * When the speak button is clicked, the audio input device is
 * started and a signal will be emitted, captured by audioIN and
 * the recorded data will be sent to the server.
 */
void MainWindow::on_speakButton_clicked()
{
    speaking = !speaking;

    if (speaking)
    {
        ui->speakButton->setText("Speaking");
        audIN->audioInput()->start();
        emit readyToRecord(client, client->getServerAddress(), client->getServerPort());
    } else {
        ui->speakButton->setText("Click to speak");
        audIN->audioInput()->stop();
    }
}

/*
 * Show some notification message (if the server emitted an error or warning signal)
 */
void MainWindow::showNotificationMessage(const QString &message)
{
    qDebug() << "Notification: " << message;
    ui->labelNotification->setText("Error: \n" + message);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::cancel()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::checkAndPlayMessage(const QHostAddress &peerAddress,
                                     const quint16 peerPort,
                                     QByteArray &datagram)
{
    if (listening)
    {
        audioOut->playMessage(datagram);
    }
}

void MainWindow::on_buttonBox_accepted()
{
    client = std::make_shared<UDPClient>(QHostAddress(ui->ipInputBox->text()), quint16(ui->portInputBox->text().toInt()));
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_buttonBox_rejected()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &ip)
{
    // If the back button was clicked, this slot is triggered because the comboBox is cleared
    // in this case, we don't want to do anything (if the ip string is empty just don't do anything)
    if (!ip.isEmpty())
    {
        QString splitIp = ip.split(":")[0];

        server = std::make_shared<UDPServer>(QHostAddress(splitIp),port);
        // Try to start server
        if(!server->startServer()) // if starting the server fails
        {
            ui->serverErrorLabel->setText("Failed to start the server");
        }
        connect(server.get(), &UDPServer::datagramReceived, this, &MainWindow::checkAndPlayMessage);
        // On error (e.g. not being able to send a datagram), the user is notified
        connect(server.get(), &UDPServer::errorMessage, this, &MainWindow::showNotificationMessage);
        connect(server.get(), &UDPServer::warningMessage, this, &MainWindow::showNotificationMessage);
    }
}

void MainWindow::on_serverBackButton_clicked()
{   
    ui->stackedWidget->setCurrentIndex(2);
    listening = false;
    ui->listenButton->setText("Click to listen");
    ui->listenButton->setChecked(false);
    audioOut.reset();
    server.reset();

    ui->comboBox->clear();

}

void MainWindow::on_clientBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    audIN.reset();
    speaking = false;
    ui->speakButton->setText("Click to speak");
}

void MainWindow::on_listenButton_toggled(bool checked)
{
    if (checked)
    {
        listening = true;
        ui->listenButton->setText("Click to stop listening");
    } else
    {
        listening = false;
        ui->listenButton->setText("Click to listen");
    }
}
