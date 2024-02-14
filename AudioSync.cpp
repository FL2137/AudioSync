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

    runServer();

    uiConnects();
}

void AudioSync::uiConnects() {

    //connect dial to volume function and "LCD" display
    connect(ui.volumeDial, &QDial::valueChanged, this, [=](int value) {
        session->changeRenderVolume(value);
    });

    connect(ui.dialogButton, &QPushButton::clicked, this, &AudioSync::runConnectDialog);

    connect(ui.connectButton, &QPushButton::clicked, this, [this]() {
        int DEFAULT_PORT = 3002;

        session = std::make_unique<Session>(localAddress, DEFAULT_PORT);
        connect(session.get(), &Session::runLoginDialog, this, &AudioSync::runLoginDialog);
        
        // session = new Session(localAddress, ui.portLineEdit->text().toInt());
        session->startSession();

    });

    connect(ui.testRequest, &QPushButton::clicked, this, [this]() {
        
    });

    connect(ui.createRoomButton, &QPushButton::clicked, this, [this]() {
            
        std::string response;

        TcpRequest request;
        request.type = "CREATEROOM";
        request.uid = session.get()->uid;

        TcpClient::send(request, response);

        if (json::accept(response)) {
            json res = json::parse(response);
            if (res["OK"] == "ok") {
                session.get()->roomid = res["rid"].get<int>();
                
                //put own AvatarWidget into room lobby
            }
        }
    });
}

AudioSync::~AudioSync() {

    renderThread.quit();
    renderThread.wait();

    captureThread.quit();
    captureThread.wait();
}

//functions

void AudioSync::runServer() {

    auto serverFoo = [&](std::string request, std::string& response) {
    };

    Server server(serverFoo, this); 
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
    loginDialog = new LoginDialogClass(*session, this);
}