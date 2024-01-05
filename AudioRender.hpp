#pragma once
#define WIN32_LEAN_AND_MEAN

//qt includes
#include <qbytearray.h>
#include <qdebug.h>
#include <qobject.h>
#include <qbuffer.h>
#include <qaudiodevice.h>
#include <qaudiosink.h>
#include <qmediadevices.h>
#include <qfile.h>
#include <qsemaphore.h>
#include <qmutex.h>
#include <qthread.h>

//win32 includes
#include <Windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <mmreg.h>
#include <guiddef.h>
#include <functiondiscoverykeys.h>
#include <setupapi.h>
#include <mmiscapi.h>
#include <assert.h>
#include <comdef.h>
#include <mmeapi.h>

#define BUFFER_SIZE 1764

class AudioRender : public QObject {

	Q_OBJECT

public:

	AudioRender(QMutex *mutex) {
		this->mutex = mutex;
	}

	QMutex *mutex;

public slots:
	void render(QByteArray* buffer);
	void win32Render(char *buffer);

signals:
	void bufferRendered();

private:
	void handleStateChange(QAudio::State state);

private:
	QAudioSink* sink;
	QBuffer* qbuffer;
	QFile* file;
	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 
};