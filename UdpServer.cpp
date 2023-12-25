#include "UdpServer.hpp"


UdpServer::UdpServer(QByteArray *targetBuffer, qint16 port) {

	this->targetBuffer = targetBuffer;

	socket = new QUdpSocket(this);
	socket->bind(QHostAddress::LocalHost, port);
	connect(socket, &QUdpSocket::readyRead, this, &UdpServer::readPendingData);
	qDebug() << "Socket.Port: " << socket->localPort() << "  Socket.Addr: " << socket->localAddress();
}


void UdpServer::readPendingData() {
	qDebug() << "UdpServer::readPendingData";
	while (socket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = socket->receiveDatagram();
		qDebug() << "data: " << datagram.data().size();
		*targetBuffer = datagram.data();
		//processData(datagram);
	}
}


void UdpServer::sendDatagram(QByteArray *data, const QHostAddress &address, qint16 port) {
	try {

		auto ret = socket->writeDatagram(*data, address, port);
		qDebug() << "sent " << ret;

	}
	catch (const std::exception e) {
		qDebug() << e.what();
	}
}