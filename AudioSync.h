#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AudioSync.h"
#include "ui_ConnectDialog.h"

#include "InputDialog.hpp"

#include "AudioHandler.hpp"
#include "AudioFormat.hpp"
#include "Session.hpp"

#include <boost/asio.hpp>

#include <qaudiosink.h>
#include <qrandom.h>
#include <qthreadpool.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qlistview.h>
#include <qstyle.h>
#include <qdial.h>
#include <qlcdnumber.h>
#include <qmutex.h>

class AudioSync : public QMainWindow
{
    Q_OBJECT

public:
    AudioSync(QWidget *parent = nullptr);
    ~AudioSync();
    

public slots:
    void startRecording();
    void startPlaying();
    void runConnectDialog();

signals:
    void runRecordingThread();
    void runRenderingThread(char* buffer);

//private functions
private:

    void uiConnects();

//private variables
private:
    Ui::AudioSyncClass ui;
    Ui::ConnectDialog ui_connectDialog;

    QThread captureThread;
    QThread renderThread;

    QMutex captureMutex;
    QMutex renderMutex;
    QMutex serverMutex;

    mutable QString localAddress = "";

//public variables
public:
    QByteArray captureBuffer;
    char* renderBuffer; 

    ConnectDialogClass *connectDialog;
    QString address;
    int port;

    Session* session;
};