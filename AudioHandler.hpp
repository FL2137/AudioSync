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
#include <qmediadevices.h>
#include <qdebug.h>
#include <qbuffer.h>
#include <qtimer.h>
#include <qobject.h>
#include <qsoundeffect.h>

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


//This class handles audio capturing and rendering using QtMultiMedia features
class AudioHandler : public QObject {

	Q_OBJECT

public:

	void renderAudio(uint16_t* audioBuffer, int audioBufferSize);

	void captureAudio(uint16_t* audioBuffer, int audioBufferSize);

	HRESULT winAudioCapture(uint16_t* audioBuffer, int audioBufferSize);
	HRESULT winAudioRender(uint16_t* audioBuffer, int audioBufferSize);



	QBuffer targetFile;
	QAudioSource* audio;

public slots:
	void AudioCaptureState(QAudio::State newState);
	void stopActivity();

private:
	void copyAudioData(uint16_t* target, uint8_t* source, int framesToWrite, int blockSize, bool& finish);

	uint16_t* sharedBuffer;
	uint32_t recordBufferIter = 0;

#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);


    HRESULT WriteWHeader(HMMIO hFile, LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData) {
        MMRESULT result;

        // make a RIFF/WAVE chunk
        pckRIFF->ckid = MAKEFOURCC('R', 'I', 'F', 'F');
        pckRIFF->fccType = MAKEFOURCC('W', 'A', 'V', 'E');

        result = mmioCreateChunk(hFile, pckRIFF, MMIO_CREATERIFF);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"RIFF/WAVE\") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'fmt ' chunk (within the RIFF/WAVE chunk)
        MMCKINFO chunk;
        chunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
        result = mmioCreateChunk(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // write the WAVEFORMATEX data to it
        LONG lBytesInWfx = sizeof(WAVEFORMATEX) + pwfx->cbSize;
        LONG lBytesWritten =
            mmioWrite(
                hFile,
                reinterpret_cast<PCHAR>(const_cast<LPWAVEFORMATEX>(pwfx)),
                lBytesInWfx
            );
        if (lBytesWritten != lBytesInWfx) {
            wprintf(L"mmioWrite(fmt data) wrote %u bytes; expected %u bytes", lBytesWritten, lBytesInWfx);
            return E_FAIL;
        }

        // ascend from the 'fmt ' chunk
        result = mmioAscend(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"fmt \" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'fact' chunk whose data is (DWORD)0
        chunk.ckid = MAKEFOURCC('f', 'a', 'c', 't');
        result = mmioCreateChunk(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // write (DWORD)0 to it
        // this is cleaned up later
        DWORD frames = 0;
        lBytesWritten = mmioWrite(hFile, reinterpret_cast<PCHAR>(&frames), sizeof(frames));
        if (lBytesWritten != sizeof(frames)) {
            wprintf(L"mmioWrite(fact data) wrote %u bytes; expected %u bytes", lBytesWritten, (UINT32)sizeof(frames));
            return E_FAIL;
        }

        // ascend from the 'fact' chunk
        result = mmioAscend(hFile, &chunk, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"fact\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        // make a 'data' chunk and leave the data pointer there
        pckData->ckid = MAKEFOURCC('d', 'a', 't', 'a');
        result = mmioCreateChunk(hFile, pckData, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioCreateChunk(\"data\") failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        return S_OK;
    }

    HRESULT FinishWFile(HMMIO hFile, MMCKINFO* pckRIFF, MMCKINFO* pckData) {
        MMRESULT result;

        result = mmioAscend(hFile, pckData, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"data\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        result = mmioAscend(hFile, pckRIFF, 0);
        if (MMSYSERR_NOERROR != result) {
            wprintf(L"mmioAscend(\"RIFF/WAVE\" failed: MMRESULT = 0x%08x", result);
            return E_FAIL;
        }

        return S_OK;
    }

    int WinReadFile(const char* filename, void** data, uint32_t* bytes_read) {
        HANDLE file = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

        if (file == INVALID_HANDLE_VALUE) return -1;

        DWORD file_size = GetFileSize(file, 0);
        if (!file_size) return -2;

        *data = HeapAlloc(GetProcessHeap(), 0, file_size + 1);

        if (!*data) return -3;


        if (!ReadFile(file, *data, file_size, (LPDWORD)bytes_read, 0))
            return -4;

        CloseHandle(file);

        ((uint8_t*)*data)[file_size] = 0;

        return 0;
    }

    void WinFreeFile(void* data) {
        HeapFree(GetProcessHeap(), 0, data);
    }




};