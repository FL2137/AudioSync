#include "AudioFormat.hpp"



AudioFormat::AudioFormat(QAudioDevice device) {
	auto format = device.preferredFormat();
	qDebug() << format.sampleRate();
	qDebug() << format.channelCount();
	qDebug() << format.sampleFormat();
}