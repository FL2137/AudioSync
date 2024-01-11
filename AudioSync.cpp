#include "AudioSync.h"
AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{

    const int size = 1764; //standard packet length generated by IAudioCaptureClient
    ///////////2420208
    renderBuffer = new char[size] { 0 };

    ui.setupUi(this);

    ui.portLineEdit->setText("3002");
    ui.hostLineEdit->setText("192.168.1.109");

    capturer = new AudioCapture(&captureMutex);
    renderer = new AudioRender(&renderMutex, &serverMutex);

    capturer->moveToThread(&captureThread);
    connect(ui.recordButton, &QPushButton::clicked, this, &AudioSync::startRecording);
    connect(this, &AudioSync::runRecordingThread, capturer, &AudioCapture::win32AudioCapture);
    connect(capturer, &AudioCapture::bufferFilled, this, &AudioSync::signalFilled);
    captureThread.start();

    renderer->moveToThread(&renderThread);
    connect(ui.playButton, &QPushButton::clicked, this, &AudioSync::startPlaying);
    connect(this, &AudioSync::runRenderingThread, renderer, &AudioRender::win32Render);
    renderThread.start();

    connect(ui.connectButton, &QPushButton::clicked, this, [this]() {
        server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, ui.portLineEdit->text().toShort(), localAddress);
        server->setSemaphores(renderer->renderSem, renderer->serverSem);
        capturer->setServer(server);
        server->readPendingData();

        ui.connectButton->setEnabled(false);
    });

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

