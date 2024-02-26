#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AudioSync.h"
#include "ui_ConnectDialog.h"
#include "Session.hpp"
#include "AudioHandler.hpp"

#include <boost/asio.hpp>

#include <qaudiosink.h>
#include <qrandom.h>
#include <qthreadpool.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qlistview.h>
#include <qfile.h>
#include <qstyle.h>
#include <qdial.h>
#include <qlcdnumber.h>
#include <qmutex.h>

#include <functional>
#include <utility>
#include <memory>

#include "TcpServer.hpp"
#include "TcpClient.hpp"

#include "InputDialog.hpp"
#include "LoginDialogClass.hpp"

#include "AvatarWidget.hpp"

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
    void runLoginDialog();

signals:
    void runRecordingThread();
    void runRenderingThread(char* buffer);

//private functions
private:
    void roomCheck();
    void friendListCheck();
    void runServer();
    void uiConnects();

//private variables
private:
    Ui::AudioSyncClass ui;
    Ui::ConnectDialog ui_connectDialog;
    Ui::LoginDialog ui_loginDialog;

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
    LoginDialogClass* loginDialog;

    AvatarWidget* myAvatar = nullptr;
    std::vector<AvatarWidget*> roomUsers = {};

    QString address;
    int port;

    std::unique_ptr<Session> session;

    int uid;                                                
};