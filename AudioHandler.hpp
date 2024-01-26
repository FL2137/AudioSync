#pragma once
#define _USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <qstring.h>
#include <qfile.h>
#include <qaudiosource.h>
#include <qaudiosink.h>
#include <qmediadevices.h>
#include <qdebug.h>
#include <qbuffer.h>
#include <qtimer.h>
#include <qobject.h>
#include <qsoundeffect.h>
#include <qthread.h>

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

#include <boost/asio.hpp>

using boost::asio::ip::udp;

class AudioHandler : public QObject {
	Q_OBJECT
public:

	enum MODE {
		CAPTURE = 0,
		RENDER = 1
	};

	AudioHandler(MODE mode);

	~AudioHandler();

	inline void setEndpoint(std::string address, std::string _port) {
		this->targetAddress = address;
		this->targetPort = _port;
	}

	float changeVolume(float newVolume);

public:
	bool initialized = false;

	inline void appendEndpoint(QString address, int port) {
		endpointList.append(udp::endpoint(boost::asio::ip::make_address(address.toStdString()), port));
	}

public slots:
	void win32AudioCapture();
	void win32Render(char *buffer);

signals:
	void bufferFilled();

	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 

//private functions
private:

	void initWASAPI(MODE mode);
	void hrHandler(HRESULT hr);

	WAVEFORMATEX* checkFormatSupport();

	//private variables
private:

	//wasapi interfaces
	IMMDevice* device = nullptr;
	IMMDeviceCollection* deviceList = nullptr;
	IAudioClient3* audioClient = nullptr;
	IAudioCaptureClient* captureClient = nullptr;
	IAudioRenderClient* renderClient = nullptr;

	WAVEFORMATEX* format = nullptr;


	//boost stuff
	std::unique_ptr<boost::asio::io_context> ioc;
	std::unique_ptr<udp::resolver> resolver;

	QList<udp::endpoint> endpointList;

	std::string targetAddress, targetPort;

	int CURRENT_COINIT = COINIT_SPEED_OVER_MEMORY;
	const int REFTIMES_PER_MILLISEC = 10000;
	const int REFTIMES_PER_SEC = 10000000;

};