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