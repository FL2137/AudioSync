#pragma once

#include <qbytearray.h>
#include <qudpsocket.h>
#include <qhostaddress.h>
#include <qobject.h>
#include <qnetworkdatagram.h>
#include <qbytearray.h>
#include <qstring.h>

class UdpServer : public QObject {
	Q_OBJECT
public:

	UdpServer() {}

	UdpServer(QByteArray *targetBuffer, qint16 port);

	void sendDatagram(QByteArray* data, const QHostAddress& address, qint16 port);

public slots: 
	void readPendingData();

private:
	QUdpSocket* socket = nullptr;
	QByteArray* targetBuffer = nullptr;
};