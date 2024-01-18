#pragma once

#include <qobject.h>
#include <qstring.h>
#include "AudioRender.hpp"
#include "AudioCapture.hpp"
#include "AudioHandler.hpp"
#include "UdpServer.hpp"


class User {

public:
	User() {

	}

};

class Session : public QObject {

	Q_OBJECT

public:

	Session(QString _address, int _port);

	~Session();
	
	inline void setServer(UdpServer* _server) {
		this->server = _server;
	}
	
	inline char* getBuffer() { return renderBuffer; }

	void startSession();

	void appendTargetEndpoint(QString address, int port);

public slots:
	void changeRenderVolume(int newVolume);
	void changeCaptureVolume(int newVolume);


signals:
	void runAudioRender(char* renderBuffer);
	void runAudioCapture();
	void runServerThread();

private:

	int id;

	User users[2];

	AudioCapture *audioCapture;
	AudioRender *audioRender;

	AudioHandler* audioC;
	AudioHandler* audioR;


	QThread renderThread;
	QThread captureThread;
	QThread serverThread;

	QMutex renderMutex;
	QMutex serverMutex;

	UdpServer* server;

	const int BUFFERSIZE = 1764;
	char* renderBuffer;

	int port;
	QString address;

};