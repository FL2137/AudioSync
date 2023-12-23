#include "AudioSync.h"
AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
    int size = 1000000;
    ///////////2420208
    audioBuffer.resize(size);
    ui.setupUi(this);

    ui.portLineEdit->setText("3002");

    server = new UdpServer(&audioBuffer, ui.portLineEdit->text().toShort());
    capturer = new AudioCapture();
    renderer = new AudioRender();

    capturer->moveToThread(&captureThread);
    connect(ui.recordButton, &QPushButton::clicked, this, &AudioSync::startRecording);
    connect(this, &AudioSync::runRecordingThread, capturer, &AudioCapture::winAudioCapture);
    connect(capturer, &AudioCapture::bufferFilled, this, &AudioSync::signalFilled);
    captureThread.start();

    connect(ui.playButton, &QPushButton::clicked, this, &AudioSync::startPlaying);

    connect(ui.connectButton, &QPushButton::clicked, server, &UdpServer::readPendingData);

}

AudioSync::~AudioSync() {

    captureThread.quit();
    captureThread.wait();

    //lala
    delete server;
    delete capturer;
    delete renderer;
}

void AudioSync::startPlaying() {
    renderer->render(&audioBuffer);
}

void AudioSync::startRecording() {
    qDebug() << "sending datagram";
    QByteArray bff("test data");
    server->sendDatagram(&bff, QHostAddress::LocalHost, 3002);
    //emit this->runRecordingThread(&audioBuffer);
}


void AudioSync::signalFilled() {
    QByteArray bff("laflalfafl");
    server->sendDatagram(&bff, QHostAddress::LocalHost, 3002);
}
