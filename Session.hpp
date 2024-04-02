#pragma once

#include <tuple>

#include <qobject.h>
#include <qstring.h>

#include "AudioHandler.hpp"
#include "HolePuncher.hpp"

class Session : public QObject {

	Q_OBJECT

public:

	Session(int _uid);

	~Session();
	
	inline char* getBuffer() { return renderBuffer; }

	void startSession();

	void appendTargetEndpoint(QString address, int port);

	void setUserCreds(QString nick);

public slots:
	void changeRenderVolume(int newVolume);
	void changeCaptureVolume(int newVolume);

signals:
	void runAudioRender(char* renderBuffer);
	void runAudioCapture();
	void runServerThread();
	void runLoginDialog();

private:

public:
	int roomid;
	int uid;
	QString nick;

private:

	AudioHandler* audioCapture;
	AudioHandler* audioRender;

	QThread renderThread;
	QThread captureThread;

	const int BUFFERSIZE = 1764;
	char* renderBuffer;
};