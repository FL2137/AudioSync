#include "AudioHandler.hpp"


AudioHandler::AudioHandler(MODE mode) {
	initWASAPI(mode);
}

AudioHandler::~AudioHandler() {
	CoUninitialize();
	delete format;
}

void AudioHandler::win32AudioCapture() {
	HRESULT hr;
	REFERENCE_TIME hnsActualDuration;

	uint32_t bufferFrameCount = 0;
	uint32_t nFramesAvailable;
	uint32_t packetLength = 0;
	uint8_t* data;
	DWORD flags;

	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC * 2;

	unsigned int bufferSize;
	audioClient->GetBufferSize(&bufferSize);

	hr = audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient);
	hrHandler(hr);

	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferSize / format->nSamplesPerSec;
	audioClient->Start();

	bool finish = false;

	while (!finish) {
		//Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		hr = captureClient->GetNextPacketSize(&packetLength);

		while (packetLength != 0) {
			hr = captureClient->GetBuffer(&data, &nFramesAvailable, &flags, NULL, NULL);
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				data = NULL;

			long bytesToWrite = nFramesAvailable * format->nBlockAlign;
			//qDebug() << "BytesToWrite: " << bytesToWrite;

			if (data == NULL) {
				qDebug() << "data NULL";
			}
			else {
				//qDebug() << "bytesToWrite: " << bytesToWrite;
				server->sendDatagrams((char*)data, bytesToWrite);
			}
			hr = captureClient->ReleaseBuffer(nFramesAvailable);
			hr = captureClient->GetNextPacketSize(&packetLength);
		}
	}
}

void AudioHandler::win32Render(char* buffer) {

	HRESULT hr;

	uint32_t bufferSizeFrames = 0;
	audioClient->GetBufferSize(&bufferSizeFrames);

	hr = audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));
	hrHandler(hr);

	REFERENCE_TIME hnsActualDuration = (double)REFTIMES_PER_SEC * bufferSizeFrames / format->nSamplesPerSec;

	audioClient->Start();

	double playbackTime = 0.0;
	const float TONE_HZ = 440;
	const int16_t TONE_VOLUME = 3000;
	int samplesIterator = 0;

	bool run = true;
	while (run) {
		//Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		uint32_t bufferPadding;
		audioClient->GetCurrentPadding(&bufferPadding);

		uint32_t soundBufferLatency = bufferSizeFrames / 50;
		uint32_t nFramesToWrite = soundBufferLatency - bufferPadding;

		int16_t* renderBuffer;

		renderClient->GetBuffer(nFramesToWrite, (BYTE**)(&renderBuffer));
		samplesIterator = 0;

		int bytesToWrite = nFramesToWrite * format->nBlockAlign;
		if (bytesToWrite != 0) {
			renderMutex->lock();
			std::memcpy(renderBuffer, buffer, bytesToWrite);
			renderMutex->unlock();
		}

		renderClient->ReleaseBuffer(nFramesToWrite, 0);
	}
	audioClient->Stop();
}

void AudioHandler::initWASAPI(MODE mode) {
	HRESULT hr = CoInitialize(NULL);
	//hrHandler(hr);
	IMMDeviceEnumerator* deviceEnum = nullptr;

	//audio output is still defaulted to the currently playing one
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnum);
	hrHandler(hr);
	hr = deviceEnum->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	hrHandler(hr);

	IMMDeviceCollection* col;
	deviceEnum->EnumAudioEndpoints(eRender, eConsole, &col);
	if (col != nullptr) {
		col->Release();
	}

	deviceEnum->Release();
	hr = device->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (void**)&audioClient);
	hrHandler(hr);
	
	audioClient->GetMixFormat(&format);

	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nChannels = 2;
	format->nSamplesPerSec = 44100;
	format->nBlockAlign = (format->nChannels * format->wBitsPerSample) / 8;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
	format->cbSize = 0;

	//check format support
	//auto formatCheck = checkFormatSupport();

	/*if (format != formatCheck) {
		initialized = false;
	}S*/

	REFERENCE_TIME requestedBufferDuration = REFTIMES_PER_SEC * 2;

	DWORD streamflags;
	if (mode == CAPTURE) {
		streamflags = AUDCLNT_STREAMFLAGS_LOOPBACK;
	}
	else if (mode == RENDER) {
		//streamflags = (AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
		streamflags = 0;
	}
	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamflags, requestedBufferDuration, 0, format, NULL);

	initialized = true;
}

float AudioHandler::changeVolume(float newVolume) {
	ISimpleAudioVolume* audioVolume;
	audioClient->GetService(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&audioVolume));
	float value;
	audioVolume->SetMasterVolume(((float)newVolume / 10.f), NULL);
	audioVolume->GetMasterVolume(&value);
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

