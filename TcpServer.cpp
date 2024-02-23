#include "TcpServer.hpp"

Server::Server(std::function<void(std::string, std::string&)> parseFun, QObject *parent) : QObject(parent) {
	
	this->parseFunction = parseFun;
	
	tcpServer = new QTcpServer(this);

	connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);

	if (tcpServer->listen(QHostAddress::AnyIPv4, 3009)) {
		qDebug() << "Server started";
	}
	else {
		qWarning() << "Server could not start";
	}
}

void Server::newConnection() {

	tcpServer->pauseAccepting();

	QTcpSocket* socket = tcpServer->nextPendingConnection();

	std::string response;

	if (socket->waitForReadyRead(3000)) {
		QByteArray data = socket->readAll();
		parseFunction(data.toStdString(), response);
		socket->write(response.data(), response.size());
	}

	socket->close();

	tcpServer->resumeAccepting();
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