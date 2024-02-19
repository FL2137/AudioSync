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

    //connect dial to volume function and "LCD" display
    connect(ui.volumeDial, &QDial::valueChanged, this, [=](int value) {
        session->changeRenderVolume(value);
    });

    connect(ui.dialogButton, &QPushButton::clicked, this, &AudioSync::runConnectDialog);

    runLoginDialog();

    connect(ui.createRoomButton, &QPushButton::clicked, this, [this]() {

        int DEFAULT_PORT = 3002;
        session = std::make_unique<Session>(localAddress, DEFAULT_PORT);
        session->startSession();

        TcpRequest request;
        request.type = "CREATEROOM";
        request.uid = session.get()->uid;
        json response;

        TcpClient::send(request, response);

        if (response["ok"] == "OK") 
             session.get()->roomid = response["rid"].get<int>();
    });
}

AudioSync::~AudioSync() {

    renderThread.quit();
    renderThread.wait();

    captureThread.quit();
    captureThread.wait();

    delete loginDialog;
    delete myAvatar;
}

//functions

void AudioSync::runServer() {

    auto serverFoo = [&](std::string request, std::string& response) {
        json js;
        if (json::accept(request)) {
            js = json::parse(request);
        }
        

        if (js["type"] == "NOTIFY") {
            //call for room check
            roomCheck();
            response = "{'OK':'ok'}";
        }

    };

    Server server(serverFoo, this); 
}

void AudioSync::roomCheck() {
    TcpRequest request;
    request.type = "ROOMCHECK";
    json data;
    data["uid"] = session.get()->uid;
    data["rid"] = session.get()->roomid;

    json response;
    TcpClient::send(request, response);

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
    qDebug() << "AudioSync::runLoginDialog() called";
    loginDialog = new LoginDialogClass(this);
    connect(loginDialog, &LoginDialogClass::passUid, this, [this](int uid) {
        this->uid = uid;
    });
}