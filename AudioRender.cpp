#include "AudioRender.hpp"


void AudioRender::render(QByteArray *audioArray) {
	QBuffer buffer(audioArray, nullptr);
	buffer.open(QIODevice::ReadOnly);

	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleFormat(QAudioFormat::Int16);

	QAudioDevice info(QMediaDevices::defaultAudioOutput());
	if (!info.isFormatSupported(format)) {
		qDebug() << "Default format is not supported";
	}

	sink = new QAudioSink(format, nullptr);
	sink->start(&buffer);
}