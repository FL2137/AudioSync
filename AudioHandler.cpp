#include "AudioHandler.hpp"

using boost::asio::ip::udp;	

AudioHandler::AudioHandler(MODE mode) {
	
}

AudioHandler::~AudioHandler() {

	delete format;
}

void AudioHandler::win32AudioCapture() {

}

void AudioHandler::win32Render() {

	
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

