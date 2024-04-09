#include "AudioSync.h"

QList<QString> listLocalAddresses() {
    using boost::asio::ip::tcp;
    
    QList<QString> addressList = {};
    boost::asio::io_service ios;
    tcp::resolver resolver(ios);
    tcp::resolver::query query(boost::asio::ip::host_name(), "");
    tcp::resolver::iterator iter = resolver.resolve(query);
    tcp::resolver::iterator end;
    while (iter != end) {
        tcp::endpoint ep = *iter++;
        if(ep.address().is_v4())
            addressList.append(QString::fromStdString(ep.address().to_string()));
    }
    return addressList;
}

AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QStringList addressList = listLocalAddresses();

    localAddress = addressList[addressList.count() - 1];

    myAvatar = new AvatarWidget(this);

    runServer();

    uiConnects();
}

void AudioSync::uiConnects() {

    connect(ui.createRoomButton, &QPushButton::clicked, this, &AudioSync::createRoom);

    connect(ui.okButton, &QPushButton::clicked, this, &AudioSync::checkCredentials);

}

void AudioSync::createRoom() {

    json request;
    request["type"] = "CREATE_ROOM";
    request["uid"] = this->uid;

    std::string res;

    BeastClient::syncBeast(request.dump(), res);

    json response = json::parse(res);

    if (response["ok"] == "OK") {
        session = std::make_unique<Session>(this->uid);

        //room gui update here
        //##
    }
    else {
        QMessageBox::warning(this, "Failure creating your lobby", "Server rejected the creation of your lobby room");
    }


}

AudioSync::~AudioSync() {

    delete myAvatar;

    for (AvatarWidget* avatar : roomUsers) { //ensure deletion of every avatar widget
        delete avatar;
    }

    delete server;
    delete webSocketClient;
}

//functions

void AudioSync::runServer() {


    auto serverFoo = [&](std::string request, std::string& response) {
        
        json body = json::parse(request);


        if ((body["type"].get<std::string>().find("NOTIFY") != std::string::npos) && body["uid"].get<int>() == this->uid)
            return;
        
        //notifications, server sent these on it's own
        if (body["type"] == "NOTIFY_ROOM") {

            roomCheck();
        }
        else if (body["type"] == "NOTIFY_FRIENDS") {

            friendListCheck();
        }


        //responses to requests sent by this client application
        else if (body["type"] == "RESPONSE_ROOMCHECK") {
            if (body["ok"] == "OK") {
                qDebug() << "roomcheck fine";
            }
        }
        else if (body["type"] == "RESPONSE_CREATE_ROOM") {

        }
        else if (body["type"] == "RESPONSE_LOGIN") {
            if (body["ok"] == "OK") {
                qDebug() << "login fine";
                ui.loginShade->deleteLater();
            }
            else {
                ui.badLoginEdit->setText("Incorrect login or password");
                ui.passEdit->setText("");
                ui.loginEdit->setText("");
            }
        }
    };

    QUrl url("192.168.0.109:3005");
    webSocketClient = new WebsocketClient(url, serverFoo, this);
    connect(this, &AudioSync::sendWebSocketMessage, webSocketClient, &WebsocketClient::emitTextMessage);
}

void AudioSync::roomCheck() {

    json request;

    request["type"] = "ROOM_CHECK";

    json data;
    data["uid"] = session.get()->uid;
    data["rid"] = session.get()->roomid;

    request["data"] = data;

    std::string strResponse;

    emit sendWebSocketMessage(request.dump());
}

void AudioSync::friendListCheck() {
   
    json request;
    request["type"] = "FRIENDS_CHECK";
    request["uid"] = this->uid;
    
    std::string res;
    BeastClient::syncBeast(request.dump(), res);
    json response;
    if (json::accept(res)) {
        response = json::parse(res);
    }

    std::vector<std::string> frens = {};

    frens = response["data"].get<std::vector<std::string>>();

    //check if someone become online

    for (auto f : frens) {
        if (ui.frenList->findItems(QString::fromStdString(f), Qt::MatchFlag::MatchExactly).count() == 0)
            ui.frenList->addItem(new QListWidgetItem(QString::fromStdString(f), ui.frenList));
    }

    //check if somone went offline
    std::vector<QListWidgetItem*> toRemove = {};

    for (int item = 0; item < ui.frenList->count(); item++) {
        if (std::find(frens.begin(), frens.end(), ui.frenList->item(item)->text().toStdString()) == frens.end()) {
            toRemove.push_back(ui.frenList->item(item));
        }
    }

    for (QListWidgetItem* item : toRemove) {
        ui.frenList->removeItemWidget(item);
    }
}

//slots and signals
void AudioSync::startPlaying() {
    emit runRenderingThread(renderBuffer);
}

void AudioSync::startRecording() {
    emit runRecordingThread();
}

void AudioSync::runConnectDialog() {
    connectDialog = new ConnectDialogClass(*session, this);
}

void AudioSync::checkCredentials() {

    QString nicknameText = ui.loginEdit->text();
    QString passwordText = ui.passEdit->text();

    json body;
    body["type"] = "LOGIN";

    json data;
    data["nickname"] = nicknameText.toStdString();
    data["password"] = passwordText.toStdString();

    body["data"] = data;

    qDebug() << body.dump();

    emit this->sendWebSocketMessage(body.dump());
}