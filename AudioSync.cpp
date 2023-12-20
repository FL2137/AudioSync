#include "AudioSync.h"
AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
    int size = 1000000;
    ///////////2420208
    audioBuffer.resize(size);

    capturer = new AudioCapture();
    renderer = new AudioRender();

    capturer->moveToThread(&captureThread);
    connect(ui.recordButton, &QPushButton::clicked, this, &AudioSync::startRecording);
    connect(this, &AudioSync::runRecordingThread, capturer, &AudioCapture::winAudioCapture);
    captureThread.start();
    
    ui.setupUi(this);

    connect(capturer, &AudioCapture::bufferFilled, this, &AudioSync::signalFilled);
    connect(ui.playButton, &QPushButton::clicked, this, &AudioSync::startPlaying);
}

AudioSync::~AudioSync() {

    captureThread.quit();
    captureThread.wait();


    delete capturer;
    delete renderer;
}

void AudioSync::startPlaying() {
    renderer->render(&audioBuffer);
}

void AudioSync::startRecording() {
    emit
}


void AudioSync::signalFilled() {
    qDebug() << "sraka";
    ui.label->setStyleSheet("{background-color:red}");

}