#include "AudioCapture.hpp"
#include <iostream>


void AudioCapture::win32AudioCapture(QByteArray* buffer) {
	HRESULT hr;
	qDebug() << "winAudioCapture()";
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

	int bufferSize = buffer->size();

	while (!finish) {
		//Sleep(hnsActualDuration / 10000 / 2);

		hr = captureClient->GetNextPacketSize(&packetLength);

		while (packetLength != 0) {
			qDebug() << "BufferIter:" << bufferIter;
			hr = captureClient->GetBuffer(&data, &nFramesAvailable, &flags, NULL, NULL);
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				data = NULL;
			
			long bytesToWrite = nFramesAvailable * format->nBlockAlign;
			qDebug() << "BytesToWrite: " << bytesToWrite;

			for (int div = 0; div < bytesToWrite; div += 252) {
				std::memcpy(buffer->data(), data, bytesToWrite / 7);
				server->sendDatagram(buffer, QHostAddress("192.168.1.109"), 3002);
			}

//			emit this->bufferFilled();
			
			//bufferIter += bytesToWrite;

			//if (bufferIter >= (bufferSize - 1764)) {
				//bufferIter = 0;
			//	finish = true;
			//}
			//emit here?
			//qDebug() << "bufferIter:" << bufferIter;  


			hr = captureClient->ReleaseBuffer(nFramesAvailable);
			hr = captureClient->GetNextPacketSize(&packetLength);
		}
	}
}
