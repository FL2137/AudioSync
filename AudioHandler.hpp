#pragma once

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

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
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>



class AudioHandler {



public:


	static void renderAudio(uint16_t* audioBuffer, int audioBufferSize);

	static void captureAudio(uint16_t* audioBuffer, int audioBufferSize);









private:

	void copyAudioData(uint16_t* target, uint8_t* source, int framesToWrite, int blockSize, bool& finish);

	const int REFTIMES_PER_SEC = 10000000;
	const int REFTIMES_PER_MILLISEC = 10000;

	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
};