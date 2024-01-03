#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AudioSync.h"

#include "AudioHandler.hpp"
#include "AudioRender.hpp"
#include "AudioCapture.hpp"
#include "AudioFormat.hpp"
#include "UdpServer.hpp"

#include <qaudiosink.h>
#include <qrandom.h>
#include <qthreadpool.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qlistview.h>
#include <qstyle.h>

class AudioSync : public QMainWindow
{
    Q_OBJECT

public:
    AudioSync(QWidget *parent = nullptr);
    ~AudioSync();
    QByteArray captureBuffer;
    QByteArray renderBuffer;

public slots:
    void startRecording();
    void startPlaying();
    void signalFilled();

signals:
    void runRecordingThread(QByteArray* buffer);
    void runRenderingThread(QByteArray* buffer);

//private functions
private:
    void listAudioDevices();

    void uiConnects();

//private variables
private:
    Ui::AudioSyncClass ui;

    AudioRender* renderer = nullptr;
    AudioCapture* capturer = nullptr;

    QThread captureThread;
    QThread renderThread;

    UdpServer* server = nullptr;

    QMutex captureMutex;
    QMutex renderMutex;

    mutable QString myAddress = "";

};