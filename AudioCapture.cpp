#include "AudioCapture.hpp"
#include <iostream>


void AudioCapture::win32AudioCapture() {
	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	REFERENCE_TIME hnsActualDuration;

	uint32_t bufferFrameCount = 0;
	uint32_t nFramesAvailable;
	uint32_t packetLength = 0;
	uint8_t* data;
	DWORD flags;
	
	int errorStop;
	REFERENCE_TIME hnsRequestedDuration = 10000000;

	unsigned int bufferSize;
	audioClient->GetBufferSize(&bufferSize);

	hr = audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient);
	hrHandler(hr);

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



void AudioCapture::initWASAPI() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	IMMDeviceEnumerator* enumerator = nullptr;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
	hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	enumerator->Release();

	hr = device->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, NULL, (void**)&audioClient);
	
	REFERENCE_TIME hnsRequestedDuration = 10000000;
	hr = audioClient->GetMixFormat(&format);

	//setup format
	format->wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	format->nChannels = 2;
	format->nSamplesPerSec = 44100;
	format->nBlockAlign = 4;
	format->nAvgBytesPerSec = 176400l;
	format->cbSize = 0;

	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, format, NULL);
}


void AudioCapture::hrHandler(HRESULT hr) {
	if (hr != S_OK) {
		_com_error _comerr(hr);
		std::cout << "Error: " << (char*)_comerr.ErrorMessage() << std::endl;
	}
}


float AudioCapture::changeVolume(float newVolume) {
	CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	ISimpleAudioVolume* audioVolume;
	audioClient->GetService(__uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&audioVolume));
	float value;
	audioVolume->SetMasterVolume(((float)newVolume / 10.f), NULL);
	audioVolume->GetMasterVolume(&value);
	qDebug() << "Setting Capture Volume to: " << value;
	audioVolume->Release();
	return value * 10;
	CoUninitialize();
}