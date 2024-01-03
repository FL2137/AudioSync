#include "AudioRender.hpp"

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


	mutex->lock();
	qDebug() << "Renderer.Mutex::lock()";
	sink->start(qbuffer);
	mutex->unlock();
	qDebug() << "Renderer.Mutex::unlock()";

}

void AudioRender::handleStateChange(QAudio::State state) {

	switch (state) {
		case QAudio::State::IdleState: {
			//audio data run out;
			mutex->lock();
			qDebug() << "Renderer.Mutex::lock()";
			qbuffer->seek(0);
			sink->start(qbuffer);
			mutex->unlock();
			qDebug() << "Renderer.Mutex::unlock()";
			break;
		}
		default: {
			//lalalal
			break;
		}

	}
}


void AudioRender::win32Render(QByteArray* buffer) {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	IMMDeviceEnumerator* enumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
	IMMDevice* device;
	hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	enumerator->Release();

	IAudioClient* audioClient;
	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&audioClient);
	device->Release();

	WAVEFORMATEX format = {};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16;
	format.nChannels = 2;
	format.nSamplesPerSec = 44100;
	format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	REFERENCE_TIME requestedBufferDuration = 10000000 * 2;

	DWORD streamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, requestedBufferDuration, 0, &format, nullptr);

	IAudioRenderClient* renderClient;

	hr = audioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID*)(&renderClient));

	uint32_t bufferSizeFrames = 0;
	audioClient->GetBufferSize(&bufferSizeFrames);

	audioClient->Start();


	double playbackTime = 0.0;
	const float TONE_HZ = 440;
	const int16_t TONE_VOLUME = 3000;
	int samplesIterator = 0;

	bool run = true;

	while (run) {

		uint32_t bufferPadding;
		audioClient->GetCurrentPadding(&bufferPadding);

		uint32_t soundBufferLatency = bufferSizeFrames / 50;
		uint32_t nFramesToWrite = soundBufferLatency - bufferPadding;

		int16_t* renderBuffer;
		
		renderClient->GetBuffer(nFramesToWrite, (BYTE**)(&renderBuffer));
		//qDebug() << "nframes to write: " << nFramesToWrite;
		
		samplesIterator = 0;

		qDebug() << "framesToWrite: " << nFramesToWrite;
			
		mutex->lock();
		for (uint32_t frameI = 0; frameI < nFramesToWrite; ++frameI) {

			*renderBuffer++ = buffer->data()[samplesIterator] | (buffer->data()[samplesIterator + 1] << 8);
			*renderBuffer++ = buffer->data()[samplesIterator] | (buffer->data()[samplesIterator + 1] << 8);

			//*renderBuffer++ = buffer->data()[samplesIterator++];

			samplesIterator += 4;

			if (samplesIterator >= buffer->size())
				samplesIterator = 0;

			samplesIterator %= buffer->size();
		}
		mutex->unlock();

		renderClient->ReleaseBuffer(nFramesToWrite, 0);
	}


	//uninitialize COM drivers
	CoUninitialize();

	audioClient->Stop();
	audioClient->Release();
	renderClient->Release();
}