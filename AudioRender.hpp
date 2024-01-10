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
#include <devicetopology.h>
#include "Semaphore.hpp"

#define BUFFER_SIZE 1764

class AudioRender : public QObject {

	Q_OBJECT

public:

	AudioRender(QMutex *mutex) {
		this->mutex = mutex;
		renderSem = new Semaphore(0);
		serverSem = new Semaphore(1);

		initializeWASAPI();
	}

	~AudioRender() {
		delete renderSem;
		delete serverSem;
	}

	QMutex *mutex;
	Semaphore* renderSem, *serverSem;

public slots:
	void render(QByteArray* buffer);
	void win32Render(char *buffer);
	void changeVolume(int newVolume);


signals:
	void bufferRendered();

private:
	void handleStateChange(QAudio::State state);

	void initializeWASAPI();

private:
	QAudioSink* sink;
	QBuffer* qbuffer;
	QFile* file;

	IMMDevice* device = nullptr;

	void hrHandler(HRESULT hr);

	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 
};