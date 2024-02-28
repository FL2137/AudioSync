#include "TcpServer.hpp"

Server::Server(std::function<void(std::string, std::string&)> parseFun, QObject *parent) : QTcpServer(parent) {
	
	this->parseFunction = parseFun;
	
	tcpServer = new QTcpServer(this);
	connect(tcpServer, &QTcpServer::newConnection, this, &Server::incomingConnection);

    int port = 3009;

    QHostAddress address("192.168.1.109");
	if (tcpServer->listen(address, 3009)) {
        qDebug() << "Listening on :" << tcpServer->serverAddress().toString() << tcpServer->serverPort();
	}
	else {
		qWarning() << "Server could not start";
	}
}

void Server::incomingConnection() {

    tcpServer->pauseAccepting();
	QTcpSocket* socket = tcpServer->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::stateChanged, this, &Server::stateChanged);


	tcpServer->resumeAccepting();
}

void Server::onReadyRead() {

    QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());

    QByteArray data = socket->readAll();
    qDebug() << data.toStdString();
    std::string response;
    parseFunction(data.toStdString(), response);

    qDebug() << "response: " << response;

    socket->write(response.data(), response.size());
}

void Server::stateChanged(QAbstractSocket::SocketState state) {
    if(state == QAbstractSocket::SocketState::UnconnectedState)
    { } //???
}

std::string Server::base64_encode(const std::string& in) {

    std::string out;

    int val = 0, valb = -6;
    for (uchar c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string Server::base64_decode(const std::string& in) {

    std::string out;

    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val = 0, valb = -8;
    for (uchar c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}