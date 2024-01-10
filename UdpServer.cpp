#include "UdpServer.hpp"


UdpServer::UdpServer(char *targetBuffer, QMutex *renderMutex, QMutex* serverMutex, qint16 port, const QString &address) {

	this->targetBuffer = targetBuffer;
	this->renderMutex = renderMutex;
	this->serverMutex = serverMutex;

	socket = new QUdpSocket(this);

	QHostAddress hostAddress;
	hostAddress.setAddress(address);

	socket->bind(hostAddress, port);
	connect(socket, &QUdpSocket::readyRead, this, &UdpServer::readPendingData);
	
	qDebug() << "Socket.State: " << socket->state() << "   Socket.Port: " << socket->localPort() << "  Socket.Addr: " << socket->localAddress();
}

void UdpServer::readPendingData() {
	static int iterator = 0;

	while (socket->hasPendingDatagrams()) {
		
		QNetworkDatagram datagram = socket->receiveDatagram();
		renderMutex->lock();
		std::memcpy(targetBuffer, datagram.data().data(), BUFFER_SIZE);
		renderMutex->unlock();
	}
}

void UdpServer::sendDatagram(QByteArray *data, const QHostAddress &address, qint16 port) {
	try {
		auto ret = socket->writeDatagram(*data, address, port);
		if (ret == -1) {
			qDebug() << socket->errorString();
		}
	}
	catch (const std::exception e) {
		qDebug() << e.what();
	}
}

void UdpServer::sendDatagram(char* data, qint64 size, const QHostAddress &address, qint16 port) {

	qint64 ret = socket->writeDatagram(data, size, address, port);
	if (ret == -1) {
		qWarning() << socket->errorString();
	}
}

QList<QString> UdpServer::listLocalAddresses() {
	QNetworkInterface qNetInterface;
	QList<QString> list = {};
	for (const QHostAddress& address : qNetInterface.allAddresses()) {
		if (address.protocol() == QHostAddress::IPv4Protocol && address != QHostAddress::LocalHost) {
			list.push_back(address.toString());
		}
	}
	return list;
}

void UdpServer::setSemaphores(Semaphore* renderSem, Semaphore* acquireSem) {
	this->serverSem = acquireSem;
	this->renderSem = renderSem;
}