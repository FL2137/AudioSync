#include "UdpServer.hpp"


UdpServer::UdpServer(QByteArray *targetBuffer, qint16 port) {

	//find local address 
	QNetworkInterface qNetInterface;
	for (const QHostAddress& address : qNetInterface.allAddresses()) {
		if (address.protocol() == QHostAddress::IPv4Protocol && address != QHostAddress::LocalHost) {
			qDebug() << address.toString();
		}
	}

	this->targetBuffer = targetBuffer;

	socket = new QUdpSocket(this);
	QHostAddress addr;
	addr.setAddress("192.168.1.109");
	socket->bind(addr, port);
	connect(socket, &QUdpSocket::readyRead, this, &UdpServer::readPendingData);
	qDebug() << "Socket.Port: " << socket->localPort() << "  Socket.Addr: " << socket->localAddress();
}


void UdpServer::readPendingData() {
	static int iterator = 0;

	//qDebug() << "UdpServer::readPendingData";
	while (socket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = socket->receiveDatagram();
		//qDebug() << "data: " << datagram.data().size();
		//processData(datagram);
			targetBuffer->insert(0, datagram.data());
	}
}


void UdpServer::sendDatagram(QByteArray *data, const QHostAddress &address, qint16 port) {
	try {
		auto ret = socket->writeDatagram(*data, address, port);
		if (ret == -1)
			qDebug() << socket->errorString();
		else
			qDebug() << "sent " << ret;
	}
	catch (const std::exception e) {
		qDebug() << e.what();
	}
}

QList<QHostAddress> UdpServer::listLocalAddresses() {
	QNetworkInterface qNetInterface;
	QList<QHostAddress> list = {};
	for (const QHostAddress& address : qNetInterface.allAddresses()) {
		if (address.protocol() == QHostAddress::IPv4Protocol && address != QHostAddress::LocalHost) {
			list.push_back(address);
		}
	}
	return list;
}