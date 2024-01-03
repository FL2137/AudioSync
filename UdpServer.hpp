#pragma once

#include <qbytearray.h>
#include <qudpsocket.h>
#include <qhostaddress.h>
#include <qobject.h>
#include <qnetworkdatagram.h>
#include <qbytearray.h>
#include <qstring.h>
#include <qlist.h>
#include <qnetworkinterface.h>
#include <qmutex.h>

class UdpServer : public QObject {
	Q_OBJECT
public:

	UdpServer() {}

	UdpServer(QByteArray *targetBuffer, QMutex *renderMutex, qint16 port, const QString &address = "");

	void sendDatagram(QByteArray* data, const QHostAddress& address, qint16 port);
	
	void sendDatagram(char* data, qint64 size, const QHostAddress& address, qint16 port);

	static QList<QString> listLocalAddresses();

public slots: 
	void readPendingData();

private:
	QUdpSocket* socket = nullptr;
	QByteArray* targetBuffer = nullptr;

	QMutex* renderMutex;
};