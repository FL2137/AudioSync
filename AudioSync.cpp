#include "AudioSync.h"
AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.portLineEdit->setText("3002");
    ui.hostLineEdit->setText("192.168.1.109");

    ui.addressList->addItems(UdpServer::listLocalAddresses()); 

    localAddress = ui.addressList->item(ui.addressList->count() - 1)->text(); //last address listed is usually main connection
    ui.addressLabel->setText("Current address is: " + localAddress);

    uiConnects();
}


void AudioSync::uiConnects() {
    //connect list of IP addresses
    connect(ui.addressList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        qDebug() << "Address set!";
        localAddress = item->text();
        qDebug() << localAddress;
    });

    //connect dial to volume function and "LCD" display
    connect(ui.volumeDial, &QDial::valueChanged, session, [&](int value) {
        session->changeCaptureVolume(value);
    });

    connect(ui.dialogButton, &QPushButton::clicked, this, &AudioSync::runConnectDialog);

    connect(ui.connectButton, &QPushButton::clicked, this, [this]() {
        session = new Session(localAddress, ui.portLineEdit->text().toInt());
        session->startSession();
    });

}

AudioSync::~AudioSync() {

    renderThread.quit();
    renderThread.wait();

    captureThread.quit();
    captureThread.wait();

    //lala
    delete server;
    delete capturer;
    delete renderer;
    delete session;
}

//functions


//slots and signals
void AudioSync::startPlaying() {
    emit runRenderingThread(renderBuffer);
    server->runSync = true;
    server->readPendingData();
}

void AudioSync::startRecording() {
    emit runRecordingThread();
}


void AudioSync::runConnectDialog() {
    connectDialog = new ConnectDialogClass(*session, this);
}
