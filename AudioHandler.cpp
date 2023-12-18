#include "AudioHandler.hpp"


void AudioHandler::renderAudio(uint16_t* audioBuffer, int audioBufferSize) {
	
}


void AudioHandler::captureAudio(uint16_t* audioBuffer, int audioBufferSize) {

	targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
	if (targetFile.isOpen()) {
		qDebug() << "lol ok";

	}

	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleFormat(QAudioFormat::Int16);

	QAudioDevice info = QMediaDevices::defaultAudioInput();
	if (!info.isFormatSupported(format)) {
		qWarning() << "Selected audio format is NOT supported, using closest supported format";
	}
	else {
		qDebug() << "Format good.";
	}
	audio = new QAudioSource(format, this);

	connect(audio, &QAudioSource::stateChanged, this, &AudioHandler::AudioCaptureState);
	
	QTimer::singleShot(1000, this, &AudioHandler::stopActivity);
	
	
	audio->start(&targetFile);

	if (audio->state() == QAudio::ActiveState) {
		qDebug() << "hell yeah";
	}

}

void AudioHandler::stopActivity() {
	qDebug() << "finish...";
	audio->stop();
	targetFile.close();
	delete audio;
}

void AudioHandler::AudioCaptureState(QAudio::State newState) {
	switch (newState) {
		case QAudio::StoppedState: {
			if (audio->error() != QAudio::NoError) {
				//some error
			}
			else {
				qDebug() << "finished recording\n";
				//recording finished
			}
			break;
		}
		case QAudio::ActiveState: {

			break;
		}
		default:
			break;
	}
}



void AudioHandler::copyAudioData(uint16_t* target, uint8_t* source, int framesToWrite, int blockSize, bool& finish) {

	long bytesToWrite = framesToWrite * blockSize;
	std::memcpy(target + recordBufferIter, source, bytesToWrite);
    recordBufferIter += bytesToWrite / 2;

    if (clock() > 5 * CLOCKS_PER_SEC) //Record 10 seconds. From the first time call clock() at the beginning of the main().
        finish = true;
	//if(overflow) finish = true;
}

void AudioHandler::copyAudioData(QByteArray* target, uint8_t* source, int framesToWrite, int blockSize, bool& finish) {

    long bytesToWrite = framesToWrite * blockSize;
    std::memcpy(target->data() + recordBufferIter, source, bytesToWrite);
    recordBufferIter += bytesToWrite;

    if (clock() > (5 * CLOCKS_PER_SEC)) //Record 10 seconds. From the first time call clock() at the beginning of the main().
        finish = true;
    //if(overflow) finish = true;
}


HRESULT AudioHandler::winAudioCapture(QByteArray *audioBuffer, int audioBufferSize) {
    
    CoInitialize(0);
    HRESULT hr;

    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;

    uint32_t bufferFrameCount = 0;
    uint32_t nFramesAvailable;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;
    WAVEFORMATEX* format = nullptr;
    uint32_t packetLength = 0;

    uint8_t* pData;
    DWORD flags;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);

    hr = pAudioClient->GetMixFormat(&format);

    //setup format
    format->wFormatTag = WAVE_FORMAT_PCM;
    format->wBitsPerSample = 16;
    format->nChannels = 2;
    format->nSamplesPerSec = 44100;
    format->nBlockAlign = 4;
    format->nAvgBytesPerSec = 176400l;
    format->cbSize = 0;

    int err_stop;

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, format, NULL);

    if (hr != S_OK) {
        _com_error _cerr(hr);
        std::wcout << _cerr.ErrorMessage() << std::endl;
        std::cin >> err_stop;
    }

    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);

    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / format->nSamplesPerSec;

    //printing format
    {
        using namespace std;
        cout << "RIFF tag: " << format->wFormatTag << endl;
        cout << "Channels: " << format->nChannels << endl;
        cout << "SamplesPerSec: " << format->nSamplesPerSec << endl;
        cout << "bitsPerSample: " << format->wBitsPerSample << endl;
    }

    pAudioClient->Start();
    bool bDone = false;
    while (bDone == false) {
        Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);

        while (packetLength != 0) {
            hr = pCaptureClient->GetBuffer(&pData, &nFramesAvailable, &flags, NULL, NULL);
            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                pData = NULL;

            copyAudioData(audioBuffer, pData, nFramesAvailable, format->nBlockAlign, bDone);

            hr = pCaptureClient->ReleaseBuffer(nFramesAvailable);
            hr = pCaptureClient->GetNextPacketSize(&packetLength);
        }
    }

    hr = pAudioClient->Stop();

    CoTaskMemFree(format);
    pEnumerator->Release();
    pDevice->Release();
    pAudioClient->Release();
    pCaptureClient->Release();
    CoUninitialize();
    return S_OK;
}

HRESULT AudioHandler::winAudioRender(uint16_t* audioBuffer, int audioBufferSize) {
    


    uint16_t* samples = audioBuffer;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    IMMDeviceEnumerator* pEnumerator;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)(&pEnumerator));
    IMMDevice* pDevice;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnumerator->Release();

    IAudioClient* audioClient;
    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (LPVOID*)(&audioClient));
    pDevice->Release();


    //setup our format;
    WAVEFORMATEX fmt = {};
    fmt.wFormatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.nSamplesPerSec = 44100;
    fmt.wBitsPerSample = 16;
    fmt.nBlockAlign = ((fmt.nChannels * fmt.wBitsPerSample) / 8); // basically bytes per sample
    fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;

    REFERENCE_TIME requestedBufferDuration = REFTIMES_PER_SEC * 2;

    DWORD streamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, requestedBufferDuration, 0, &fmt, nullptr);

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

        int16_t* buffer;
        renderClient->GetBuffer(nFramesToWrite, (BYTE**)(&buffer));

        for (uint32_t frameI = 0; frameI < nFramesToWrite; ++frameI) {
            *buffer++ = samples[samplesIterator];
            *buffer++ = samples[samplesIterator];

            samplesIterator += 2;
            if (samplesIterator >= audioBufferSize)
                run = false;

            samplesIterator %=audioBufferSize;
        }


        renderClient->ReleaseBuffer(nFramesToWrite, 0);


        //trace how much of the wav file we already played
        IAudioClock* pClock;
        audioClient->GetService(__uuidof(IAudioClock), (LPVOID*)(&pClock));
        uint64_t playbackFrequency;
        uint64_t playbackPosition;
        pClock->GetFrequency(&playbackFrequency);
        pClock->GetPosition(&playbackPosition, 0);
        pClock->Release();
        uint64_t playbackPosSeconds = playbackPosition / playbackFrequency;
        uint64_t playbackPosSamples = playbackPosition / playbackFrequency;
    }


    //uninitialize COM drivers
    CoUninitialize();


    audioClient->Stop();
    audioClient->Release();
    renderClient->Release();
    return S_OK;
}
