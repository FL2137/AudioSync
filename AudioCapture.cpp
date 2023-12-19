#include "AudioCapture.hpp"
#include <iostream>


void AudioCapture::winAudioCapture(QByteArray* buffer) {
	HRESULT hr;

	hr = CoInitialize(0);

	REFERENCE_TIME hnsRequestedDuration = 10000000;
	REFERENCE_TIME hnsActualDuration;

	uint32_t bufferFrameCount = 0;
	uint32_t nFramesAvailable;
	IMMDeviceEnumerator* enumerator = nullptr;
	IMMDevice* device = nullptr;
	IAudioClient* audioClient = nullptr;
	IAudioCaptureClient* captureClient = nullptr;
	WAVEFORMATEX* format = nullptr;
	uint32_t packetLength = 0;

	uint8_t* data;
	DWORD flags;

	int bufferIter = 0;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);

	hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	enumerator->Release();
	
	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&audioClient);

	hr = audioClient->GetMixFormat(&format);

	//setup format
	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nChannels = 2;
	format->nSamplesPerSec = 44100;
	format->nBlockAlign = 4;
	format->nAvgBytesPerSec = 176400l;
	format->cbSize = 0;

	int errorStop;

	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, format, NULL);

	if (hr != S_OK) {
		_com_error _comerr(hr);
		std::wcout << _comerr.ErrorMessage() << std::endl;
		std::cin >> errorStop;
	}

	hr = audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient);

	hnsActualDuration = (double)10000000 * bufferFrameCount / format->nSamplesPerSec;

	audioClient->Start();

	bool finish = false;

	while (!finish) {
		Sleep(hnsActualDuration / 10000 / 2);

		hr = captureClient->GetNextPacketSize(&packetLength);

		while (packetLength != 0) {
			hr = captureClient->GetBuffer(&data, &nFramesAvailable, &flags, NULL, NULL);
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				data = NULL;
			
			long bytesToWrite = nFramesAvailable * format->nBlockAlign;
			std::memcpy(buffer->data() + bufferIter, data, bytesToWrite);
			bufferIter += bytesToWrite;

			if (bufferIter >= (buffer->size() - 1764)) {
				bufferIter = 0;
			}
			//emit here?
			emit this->bufferFilled();


			hr = captureClient->ReleaseBuffer(nFramesAvailable);
			hr = captureClient->GetNextPacketSize(&packetLength);
		}

	}
}
