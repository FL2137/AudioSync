#include "UdpServer.hpp"


UdpServer::UdpServer(QByteArray *targetBuffer, qint16 port) {

	this->targetBuffer = targetBuffer;

	socket = new QUdpSocket(this);
	socket->bind(QHostAddress::LocalHost, port);
	connect(socket, &QUdpSocket::readyRead, this, &UdpServer::readPendingData);
}


void UdpServer::readPendingData() {
	qDebug() << "readyRead";
	while (socket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = socket->receiveDatagram();
		qDebug() << "senderAddress: " << datagram.senderAddress();
		*targetBuffer = datagram.data();
		//processData(datagram);
	}
}


void UdpServer::sendDatagram(QByteArray *data, const QHostAddress &address, qint16 port) {
	socket->writeDatagram(*data, address, port);
}