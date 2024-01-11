#pragma once
#define USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN

#include "UdpServer.hpp"

#include <qbytearray.h>
#include <qdebug.h>
#include <qobject.h>
#include <qmutex.h>

#include <cstdio>
#include <math.h>

#include <Windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <mmreg.h>
#include <guiddef.h>
#include <devpkey.h>
#include <initguid.h>
#include <functiondiscoverykeys.h>
#include <setupapi.h>
#include <mmiscapi.h>
#include <assert.h>
#include <comdef.h>
#include <mmeapi.h>


class AudioCapture : public QObject {

	Q_OBJECT
 
public:

	AudioCapture(QMutex* mutex) {
		this->mutex = mutex;
		initWASAPI();
	}

	void setServer(UdpServer* server) {
		this->server = server;
	}

public slots:
	void win32AudioCapture();

signals:
	void bufferFilled();

	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 

//private functions
private:

	void initWASAPI();
	void hrHandler(HRESULT hr);


	//private variables
private:
	UdpServer* server = nullptr;
	QMutex* mutex = nullptr;


	//wasapi interfaces
	IMMDevice* device = nullptr;
	IMMDeviceCollection* deviceList = nullptr;
	IAudioClient3* audioClient = nullptr;
	//IAudioCaptureClient* captureClient = nullptr;

	WAVEFORMATEX* format = nullptr;
};