#pragma once
#define WIN32_LEAN_AND_MEAN

#include "UdpServer.hpp"

#include <qbytearray.h>
#include <qdebug.h>
#include <qobject.h>
#include <qmutex.h>

#include <cstdio>

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
	}

	void setServer(UdpServer* server) {
		this->server = server;
	}

private:
	UdpServer* server = nullptr;
	QMutex *mutex = nullptr;

public slots:
	void win32AudioCapture();

signals:
	void bufferFilled();

	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 
};