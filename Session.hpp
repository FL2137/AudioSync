#pragma once

#include <qobject.h>
#include <qstring.h>
#include "AudioRender.hpp"
#include "AudioCapture.hpp"
#include "AudioHandler.hpp"

class Session : public QObject {

	Q_OBJECT

public:

	Session(QString _address, int _port);

	~Session();
	
	inline char* getBuffer() { return renderBuffer; }

	void startSession();

public slots:
	void changeRenderVolume(int newVolume);
	void changeCaptureVolume(int newVolume);


signals:
	void runAudioRender(char* renderBuffer);
	void runAudioCapture();
	void runServerThread();

private:

	int id;

	AudioHandler* audioCapture;
	AudioHandler* audioRender;


	QThread renderThread;
	QThread captureThread;

	const int BUFFERSIZE = 1764;
	char* renderBuffer;

	int port;
	QString address;

};