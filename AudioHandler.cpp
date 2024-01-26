#include "AudioHandler.hpp"

using boost::asio::ip::udp;	

AudioHandler::AudioHandler(MODE mode) {
	
}

AudioHandler::~AudioHandler() {

	delete format;
}

void AudioHandler::win32AudioCapture() {
	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);

	REFERENCE_TIME hnsActualDuration;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	uint32_t bufferFrameCount;
	uint32_t nFramesAvailable;
	IMMDeviceEnumerator* enumerator = nullptr;
	IMMDevice* device = nullptr;
	IAudioClient* audioClient = nullptr;
	IAudioCaptureClient* captureClient = nullptr;
	WAVEFORMATEX* format = nullptr;
	uint32_t packetLength = 0;

	uint8_t* data;
	DWORD flags;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator), (void**)&enumerator);

	hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	enumerator->Release();

	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient);

	hr = audioClient->GetMixFormat(&format);

	if (&format == NULL) {
		std::cerr << "Capture::GetMixFormat error\n";
	}

	//setup format
	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nChannels = 2;
	format->nSamplesPerSec = 44100;
	format->nBlockAlign = (format->nChannels * format->wBitsPerSample) / 8;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
	format->cbSize = 0;

	int errorStop;

	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, format, NULL);

	audioClient->GetBufferSize(&bufferFrameCount);


	if (hr != S_OK) {
		_com_error _comerr(hr);
		std::wcout << _comerr.ErrorMessage() << std::endl;
		std::cin >> errorStop;
	}

	hr = audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient);

	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / format->nSamplesPerSec;

	audioClient->Start();

	bool finish = false;
	int bufferIter = 0;


	//udp part
	////////////////////////////////////
	boost::asio::io_context ioc;
	udp::resolver resolver(ioc);
	udp::endpoint receiverEp = *resolver.resolve(udp::v4(), "192.168.1.105", "3002");

	udp::socket socket(ioc);

	socket.open(udp::v4());


	////////////////////////////////////
	while (!finish) {

		hr = captureClient->GetNextPacketSize(&packetLength);
		while (packetLength != 0) {

			hr = captureClient->GetBuffer(&data, &nFramesAvailable, &flags, NULL, NULL);
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				data = NULL;

			long bytesToWrite = nFramesAvailable * format->nBlockAlign;
		
			
			//int sent = socket.send_to(boost::asio::buffer(data, bytesToWrite), receiverEp);

			for (const auto& ep : endpointList) {
				socket.send_to(boost::asio::buffer(data, bytesToWrite), ep);
				//qDebug() << "Sent: " << sent;
			}

			hr = captureClient->ReleaseBuffer(nFramesAvailable);
			hr = captureClient->GetNextPacketSize(&packetLength);

		}

	}
	socket.close();
	audioClient->Stop();
	captureClient->Release();
	audioClient->Release();
	device->Release();
	CoUninitialize();
}

void AudioHandler::win32Render(char *buffer) {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);

	IMMDeviceEnumerator* deviceEnum = nullptr;
	IMMDevice* device;
	//audio output is still defaulted to the currently playing one
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnum);
	hr = deviceEnum->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	deviceEnum->Release();


	IAudioClient* audioClient;
	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&audioClient);
	device->Release();


	WAVEFORMATEX* format;

	audioClient->GetMixFormat(&format);

	if (&format == NULL) {
		std::cerr << "Render::GetMixFormat error\n";
	}

	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nChannels = 2;
	format->nSamplesPerSec = 44100;
	format->nBlockAlign = (format->nChannels * format->wBitsPerSample) / 8;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
	format->cbSize = 0;
	REFERENCE_TIME requestedBufferDuration = REFTIMES_PER_SEC;

	DWORD streamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, requestedBufferDuration, 0, format, nullptr);

	IAudioRenderClient* renderClient;

	hr = audioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID*)(&renderClient));

	hrHandler(hr);

	uint32_t bufferSizeFrames = 0;
	hr = audioClient->GetBufferSize(&bufferSizeFrames);
	hrHandler(hr);

	REFERENCE_TIME hnsActualDuration = (double)10000000 * bufferSizeFrames / format->nSamplesPerSec;

	audioClient->Start();

	
	int samplesIterator = 0;

	bool run = true;


	////////////////////////////asio here
	boost::asio::io_context ioc;
	udp::socket socket(ioc, udp::endpoint(udp::v4(), 3002));

	////////////////////////////

	/*char* buffer = new char[BUFFER_SIZE  * 1000];
	for (int i = 0; i < 1000; i++) {
		udp::endpoint ep;
		int recv = socket.receive_from(boost::asio::buffer(buffer+samplesIterator, BUFFER_SIZE), ep);
		samplesIterator += BUFFER_SIZE;
		std::cout << "i;" << i << std::endl;
	}*/
	//char* buffer = new char[BUFFER_SIZE];
	while (run) {

		uint32_t bufferPadding;
		audioClient->GetCurrentPadding(&bufferPadding);

		uint32_t soundBufferLatency = bufferSizeFrames / 50;
		uint32_t nFramesToWrite = soundBufferLatency - bufferPadding;

		uint16_t* renderBuffer;

		renderClient->GetBuffer(nFramesToWrite, (BYTE**)(&renderBuffer));

		int bytesToWrite = nFramesToWrite * format->nBlockAlign;
		udp::endpoint ep;
		int rcv = socket.receive_from(boost::asio::buffer(renderBuffer, bytesToWrite), ep);
		std::cout << "received: " << rcv << std::endl;
		


		/*std::memcpy(renderBuffer, buffer + samplesIterator, bytesToWrite);
		samplesIterator += bytesToWrite;
		if (samplesIterator >= (BUFFER_SIZE * 1000) - 1764)
			run = false;*/


		renderClient->ReleaseBuffer(nFramesToWrite, 0);

	}
	//uninitialize COM drivers
	socket.close();
	audioClient->Stop();
	audioClient->Release();
	renderClient->Release();
	CoUninitialize();
	
}

float AudioHandler::changeVolume(float newVolume) {
	IAudioStreamVolume* audioVolume;

	audioClient->GetService(__uuidof(IAudioStreamVolume), reinterpret_cast<void**>(&audioVolume));
	float value;
	float* volumes = new float[format->nChannels];
	for (int i = 0; i < format->nChannels; i++)
		volumes[i] = (newVolume/10.f);

	audioVolume->SetAllVolumes(format->nChannels, volumes);
	audioVolume->GetChannelVolume(0, &value);
	
	qDebug() << "Setting Volume to: " << value;
	audioVolume->Release();
	return value * 10;
}

WAVEFORMATEX* AudioHandler::checkFormatSupport() {

	WAVEFORMATEX *closestSupported;

	HRESULT hr = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, format, &closestSupported);
	if (hr == S_OK) {
		//format supported
		delete closestSupported;
		return format;
	}
	else if (hr == S_FALSE) {
		qDebug() << "provided format not supported, using this format:";
		qDebug() << "bitsPerSample: " << closestSupported->wBitsPerSample;
		qDebug() << "channels: " << closestSupported->nChannels;
		qDebug() << "samplesPerSec: " << closestSupported->nSamplesPerSec;
		//provided format not supported, closest supported format is passed to the variable
		return closestSupported;
	}
	else if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) {
		//format not supported at all, throw smth
		throw std::exception("Audio format not supported by this device");
	}

	delete closestSupported;
	return nullptr;
}

void AudioHandler::hrHandler(HRESULT hr) {
	static int calls = 1;
	if (hr != S_OK) {
		_com_error _comerr(hr);
		MessageBox(NULL, _comerr.ErrorMessage(), L"Error", MB_OK);
		std::cout << calls++;
	}
}

