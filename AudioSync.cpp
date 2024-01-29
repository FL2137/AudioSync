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

    ui.portLineEdit->setText("3002");

    ui.addressList->addItems(listLocalAddresses()); 

    localAddress = ui.addressList->item(ui.addressList->count() - 1)->text(); //last address listed is usually main connection
    ui.addressLabel->setText("Current address is: " + localAddress);
    ui.hostLineEdit->setText(localAddress);

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
    connect(ui.volumeDial, &QDial::valueChanged, this, [=](int value) {
        session->changeRenderVolume(value);
    });

    connect(ui.dialogButton, &QPushButton::clicked, this, &AudioSync::runConnectDialog);

    connect(ui.connectButton, &QPushButton::clicked, this, [this]() {
        session = std::make_unique<Session>(localAddress, ui.portLineEdit->text().toInt());
       // session = new Session(localAddress, ui.portLineEdit->text().toInt());
        session->startSession();
    });

}

AudioSync::~AudioSync() {

    renderThread.quit();
    renderThread.wait();

    captureThread.quit();
    captureThread.wait();
}

//functions

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
