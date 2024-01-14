#pragma once

#include <qstring.h>
#include "AudioRender.hpp"
#include "AudioCapture.hpp"
#include "UdpServer.hpp"

class User {

public:
	User() {

	}

};

class Session {

public:

	Session() {
		audioCapture = new AudioCapture();
		audioRender = new AudioRender(&renderMutex, &serverMutex);

		renderBuffer = new char[BUFFERSIZE];
	}

	~Session();
	
	inline void setServer(UdpServer* _server) {
		this->server = _server;
	}

	void startSession();

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

};