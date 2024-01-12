#include "AudioSync.h"
AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.portLineEdit->setText("3002");
    ui.hostLineEdit->setText("192.168.1.109");

  
    ui.addressList->addItems(UdpServer::listLocalAddresses());
 
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
    connect(ui.volumeDial, &QDial::valueChanged, this, [this]() {
        ui.lcdVolume->display(ui.volumeDial->value());
        renderer->changeVolume(ui.volumeDial->value());
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

