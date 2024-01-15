#pragma once

#include <qobject.h>
#include <qstring.h>
#include "AudioRender.hpp"
#include "AudioCapture.hpp"
#include "UdpServer.hpp"


class User {

public:
	User() {

	}

};

class Session : public QObject {

	Q_OBJECT

public:

	Session(QString _address, int _port) {
		audioCapture = new AudioCapture();
		audioRender = new AudioRender(&renderMutex, &serverMutex);

		renderBuffer = new char[BUFFERSIZE];

		address = _address;
		port = _port;
	}

	~Session();
	
	inline void setServer(UdpServer* _server) {
		this->server = _server;
	}
	
	inline char* getBuffer() { return renderBuffer; }

	void startSession();

	void appendTargetEndpoint(QString address, int port);

signals:
	void runAudioCapture();
	void runAudioRender(char* renderBuffer);


private:

	int id;

	User users[2];

	AudioCapture *audioCapture;
	AudioRender *audioRender;

	QThread renderThread;
	QThread captureThread;

	QMutex renderMutex;
	QMutex serverMutex;

	UdpServer* server;

	const int BUFFERSIZE = 1764;
	char* renderBuffer;

	int port;
	QString address;

};