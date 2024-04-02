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

    runLoginDialog();
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
    }
    else {
        QMessageBox::warning(this, "Failure creating your lobby", "Server rejected the creation of your lobby room");
    }


}

AudioSync::~AudioSync() {

    delete loginDialog;
    delete myAvatar;

    for (AvatarWidget* avatar : roomUsers) { //ensure deletion of every avatar widget
        delete avatar;
    }

    delete server;
    delete webSocketClient;
}

//functions

void AudioSync::runServer() {

    //auto serverFoo = [&](std::string request, std::string& response) {
    //    json js;
    //    if (json::accept(request)) {
    //        js = json::parse(request);
    //    }
    //    else {
    //        qDebug() << "AudioSync::runServer(): invalid json provided";
    //    }
    //    
    //    if (js["type"] == "NOTIFY_ROOM") {
    //        //call for room check
    //        roomCheck();
    //        response = "{'ok':'OK'}";
    //    }
    //    else if (js["type"] == "NOTIFY_FRIENDS") {

    //        friendListCheck();
    //        response = "{'ok':'OK'}";
    //    }

    //};

    //server = new Server(serverFoo, this);
    
    //QUrl url("192.168.1.109:3005");
    //webSocketClient = new WebsocketClient(url, this);
}

void AudioSync::roomCheck() {

    json request;

    request["type"] = "ROOM_CHECK";

    json data;
    data["uid"] = session.get()->uid;
    data["rid"] = session.get()->roomid;

    request["data"] = data;

    std::string strResponse;

    BeastClient::syncBeast(request.dump(), strResponse);
   
    json response = json::parse(strResponse);
      
    if (response["ok"] == "OK") {
        json data = json::parse(response["data"].get<std::string>()); //for some reason this works but .get<json>() doesnt
        std::vector<std::string> users;
        data.get_to(users);
        for (const auto& user : users) {

            auto ptr = new AvatarWidget("", roomUsers.size(), ui.roomView);
            roomUsers.push_back(ptr);
        }
    }
}

void AudioSync::friendListCheck() {
    //tbi
    TcpRequest request;
    request.type = "FRIENDS_CHECK";



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

void AudioSync::runLoginDialog() {
    loginDialog = new LoginDialogClass(this);
    connect(loginDialog, &LoginDialogClass::passUid, this, [this](int uid) {
        this->uid = uid;

        /*TcpRequest request;
        request.type = "SET_AVATAR";
        request.uid = this->uid;

        QFile settings(QDir::currentPath() + "/userSettings.json");

        if (settings.open(QIODevice::ReadOnly | QIODevice::Text)) {


            json settingsJs = json::parse(settings.readAll());

            std::string avatarName = settingsJs["avatar"].get<std::string>();

            QFile avatarFile(QDir::currentPath() + "/x64/Debug/Avatars/" + QString::fromStdString(avatarName));

            if (avatarFile.open(QIODevice::ReadOnly)) {

                char* read = avatarFile.readAll().data();

                request.data["avatar"] = "avatarLalala";

                json response;

                TcpClient::send(request, response);

                if (response["ok"] == "OK") {
                    qDebug() << "avatar uploaded";
                }
            }

            avatarFile.close();
        }

        settings.close();*/
    });
}