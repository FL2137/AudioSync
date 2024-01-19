#include "AudioRender.hpp"
#include <iostream>
#include <mutex>

void AudioRender::render(QByteArray *buffer) {

	qbuffer = new QBuffer(buffer, nullptr);
	qbuffer->open(QIODevice::ReadOnly);
	
	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleFormat(QAudioFormat::Int16);

	QAudioDevice info(QMediaDevices::defaultAudioOutput());
	if (!info.isFormatSupported(format)) {
		qDebug() << "Default format is not supported";
	}
	qDebug() << "RENDERING DATA";
	sink = new QAudioSink(format, nullptr);

	connect(sink, &QAudioSink::stateChanged, this, &AudioRender::handleStateChange);
	for (int i = 0; i < 10; i++) {
		qDebug() << buffer->data()[i];
	}


	//mutex->lock();
	qDebug() << "Renderer.Mutex::lock()";
	sink->start(qbuffer);
	//mutex->unlock();
	qDebug() << "Renderer.Mutex::unlock()";

}

void AudioRender::handleStateChange(QAudio::State state) {

	switch (state) {
		case QAudio::State::IdleState: {
			//audio data run out;
			//mutex->lock();
			qDebug() << "Renderer.Mutex::lock()";
			qbuffer->seek(0);
			sink->start(qbuffer);
			//mutex->unlock();
			qDebug() << "Renderer.Mutex::unlock()";
			break;
		}
		default: {
			//lalalal
			break;
		}

	}
}

float AudioRender::changeVolume(float newVolume) {
	CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	ISimpleAudioVolume* audioVolume;
	audioClient->GetService(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&audioVolume));
	float value;
	audioVolume->SetMasterVolume(((float)newVolume/10.f), NULL);
	audioVolume->GetMasterVolume(&value);
	qDebug() << "Setting Volume to: " << value;
	audioVolume->Release();
	return value * 10;
	CoUninitialize();
}	

void AudioRender::win32Render(char *buffer) {

	HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);

	uint32_t bufferSizeFrames = 0;
	audioClient->GetBufferSize(&bufferSizeFrames);

	REFERENCE_TIME hnsActualDuration = (double)10000000 * bufferSizeFrames / format.nSamplesPerSec;
	audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));

	audioClient->Start();

	double playbackTime = 0.0;
	const float TONE_HZ = 440;
	const int16_t TONE_VOLUME = 3000;
	int samplesIterator = 0;

	bool run = true;
	while (run) {
		//Sleep((DWORD)(hnsActualDuration / 10000 / 2));

		uint32_t bufferPadding;
		audioClient->GetCurrentPadding(&bufferPadding);

		uint32_t soundBufferLatency = bufferSizeFrames / 50;
		uint32_t nFramesToWrite = soundBufferLatency - bufferPadding;

		int16_t* renderBuffer;
		
		renderClient->GetBuffer(nFramesToWrite, (BYTE**)(&renderBuffer));
		samplesIterator = 0;

		int bytesToWrite = nFramesToWrite * format.nBlockAlign;

		renderMutex->lock();
		std::memcpy(renderBuffer, buffer, bytesToWrite);
		renderMutex->unlock();


		renderClient->ReleaseBuffer(nFramesToWrite, 0);
	}
	audioClient->Stop();
	//uninitialize COM drivers
	CoUninitialize();
}


void AudioRender::initWASAPI() {
	CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	HRESULT hr; 
	IMMDeviceEnumerator* deviceEnum = nullptr;
	
	//audio output is still defaulted to the currently playing one
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnum);
	hrHandler(hr);
	hr = deviceEnum->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	hrHandler(hr);

	IMMDeviceCollection* col;
	deviceEnum->EnumAudioEndpoints(eRender, eConsole, &col);
	if(col != nullptr)
		col->Release();

	deviceEnum->Release();
	hr = device->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (void**)&audioClient);
	hrHandler(hr);

	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16;
	format.nChannels = 2;
	format.nSamplesPerSec = 44100;
	format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	WAVEFORMATEX* close;

	HRESULT formatCheck = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &format, &close);
	if (formatCheck == S_OK) {
		//git
	}
	else if (formatCheck == S_FALSE) {
		return;
	}

	REFERENCE_TIME requestedBufferDuration = 10000000;

	DWORD streamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, requestedBufferDuration, 0, &format, nullptr);

	CoUninitialize();
	initialized = true;
}

void AudioRender::hrHandler(HRESULT hr) {
	if (hr != S_OK) {
		_com_error _comerr(hr);

		auto x = _comerr.Description();
		std::cout << "Error: " << (char*)_comerr.ErrorMessage() << x <<std::endl;
	}
}