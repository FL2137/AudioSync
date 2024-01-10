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
#include "Semaphore.hpp"

#define BUFFER_SIZE 1764

class UdpServer : public QObject {
	Q_OBJECT
public:

	UdpServer() {
		
	}

	UdpServer(char *targetBuffer, QMutex *renderMutex, QMutex *serverMutex, qint16 port, const QString &address = "");

	void sendDatagram(QByteArray* data, const QHostAddress& address, qint16 port);
	
	void sendDatagram(char* data, qint64 size, const QHostAddress& address, qint16 port);

	static QList<QString> listLocalAddresses();

	void setSemaphores(Semaphore* renderSem, Semaphore* acquireSem);


public slots: 
	void readPendingData();

private:
	QUdpSocket* socket = nullptr;
	char* targetBuffer = nullptr;

	QMutex* renderMutex;
	QMutex* serverMutex;

public:
	bool runSync = false;
	Semaphore* serverSem, *renderSem;
};