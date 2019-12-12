#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), listening(true), speaking(false)
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
        // Initialize the sender handler
        senderHandler = std::make_shared<SenderHandler>(15);
        audioOut = std::make_shared<AudioOUT>();
        processor = std::make_shared<Processor>();

        std::vector<QHostAddress> addresses = UDPServer::getLocalAddresses();
        for (auto &e : addresses)
            ui->comboBox->addItem(e.toString() + ":" + QString::number(port));

        // Connect signals and slots
        connect(senderHandler.get(), &SenderHandler::newClientNotification, this, &MainWindow::addClientItem); // Add client to list if new client connects
        connect(senderHandler.get(), &SenderHandler::timeoutClientNotification, this, &MainWindow::removeClient); // Remove client from list if client timed out

        qRegisterMetaType<QByteArray>();
        processor->moveToThread(&processorThread);
        connect(&processorThread, &QThread::finished, processor.get(), &Processor::deleteLater);
        connect(this, &MainWindow::receivedData, processor.get(), &Processor::convertData);
        connect(processor.get(), &Processor::readyToPlay, audioOut.get(), &AudioOUT::playMessage);
        processorThread.start();

        ui->stackedWidget->setCurrentIndex(0);

    } else {
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
    ui->labelNotification->setText("Error: \n" + message);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::cancel()
{
    ui->stackedWidget->setCurrentIndex(2);
}


// Add a new sender to the listWidget
void MainWindow::addClientItem(const QHostAddress &peerAddress)
{
    // Create widget with sender address as label and a pushbutton
    QWidget *widg = new QWidget();

    QLabel *l = new QLabel();
    l->setText(peerAddress.toString());
    QPushButton *b = new QPushButton();
    b->setCheckable(true);
    b->setText("Listen");
    b->setObjectName(peerAddress.toString());

    // Dynamically connect the pushbutton click signal to a slot which can get the address of the specific item
    connect(b, &QPushButton::toggled, [=](bool checked) { clicked(checked, peerAddress, b->objectName()); }  );

    // Create a box layout and add the label and pushbutton
    QLayout *lay = new QHBoxLayout();
    lay->addWidget(l);
    lay->addWidget(b);

    // Set the layout of the widget to the layout containing the label and pushbutton
    widg->setLayout(lay);

    // Create an item to add to the listWidget
    QListWidgetItem *listWidget = new QListWidgetItem(ui->listWidget);
    // Automatically set the size (will not be visible otherwise)
    listWidget->setSizeHint(widg->sizeHint());
    // Set the user data to the sender address to be able to search the list easily for a client later
    listWidget->setData(Qt::UserRole, peerAddress.toString());

    // Add the item to the listWidget
    ui->listWidget->setItemWidget(listWidget, widg);

    ui->numClientsLabel->setText(QString::number(senderHandler->getNumberOfClients()));
}

// Remove a sender from the ListWidget
void MainWindow::removeClient(const QHostAddress &peerAddress)
{
    // Search the listWidget list until the sender to be removed is found
    for (int i=0; i < ui->listWidget->count(); i++)
    {
        // Get the current item from the listWidget
        QListWidgetItem * item = ui->listWidget->item(i);
        // Check if the user data is the address of the sender to be removed
        if (item->data(Qt::UserRole) == peerAddress.toString()) {
            // Remove the sender
            delete ui->listWidget->takeItem(i);
            break; // Stop the loop
        }
    }
    ui->numClientsLabel->setText(QString::number(senderHandler->getNumberOfClients()));
}

// When a button of a ListWidgetItem is toggled on, this sets the
// selectedSender to the address of the sender that this item represents.
// When the button is toggled of, the selectedSender is set to the null address
void MainWindow::clicked(bool checked, const QHostAddress &peerAddress, QString buttonName)
{
    if (checked)
    {
        QList<QPushButton *> buttons = ui->listWidget->findChildren<QPushButton *>();
        for (auto but : buttons)
        {
            if (but->objectName() != buttonName)
                but->setChecked(false);
        }
        selectedSender = peerAddress;
    } else {
        selectedSender = QHostAddress::Null;
    }
}

void MainWindow::checkAndPlayMessage(const QHostAddress &peerAddress,
                                     const quint16 peerPort,
                                     QByteArray &datagram)
{
    Q_UNUSED(peerPort)
    // Play if the message comes from the selected sender
    if (selectedSender == peerAddress)
    {
        // send this into the signal processor
        emit receivedData(datagram);
    }
}

void MainWindow::on_buttonBox_accepted()
{
    audIN = std::make_shared<AudioIN>();

    // Once the user is ready to record (i.e. the speak button has been clicked)
    // we will start sending packets to the server device
    connect(this, &MainWindow::readyToRecord, audIN.get(), &AudioIN::sendAudio);

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

        senderHandler->clearClients();
        ui->listWidget->clear();
        selectedSender = QHostAddress::Null;
        ui->numClientsLabel->setText("0");

        server = std::make_shared<UDPServer>(QHostAddress(splitIp),port);
        // Try to start server
        if(!server->startServer()) // if starting the server fails
        {
            ui->numClientsLabel->setText("Failed to start the server");
        }
        connect(server.get(), &UDPServer::datagramReceived, this, &MainWindow::checkAndPlayMessage);
        // On error (e.g. not being able to send a datagram), the user is notified
        connect(server.get(), &UDPServer::errorMessage, this, &MainWindow::showNotificationMessage);
        connect(server.get(), &UDPServer::warningMessage, this, &MainWindow::showNotificationMessage);

        connect(server.get(), &UDPServer::datagramReceived, senderHandler.get(), &SenderHandler::checkClientMembership); // Check if the client is already in the list of the handler when datagram received
    }
}

void MainWindow::on_serverBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    senderHandler.reset();
    audioOut.reset();
    server.reset();
    selectedSender = QHostAddress::Null;
    ui->listWidget->clear();
    ui->numClientsLabel->setText("0");
    ui->comboBox->clear();
}

void MainWindow::on_clientBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    audIN.reset();
    speaking = false;
    ui->speakButton->setText("Click to speak");
}
