#include "AudioRender.hpp"

void AudioRender::render(QByteArray *audioArray) {
	(audioArray, nullptr);

	buffer = new QBuffer(audioArray, nullptr);

	buffer->open(QIODevice::ReadOnly);
	
	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleFormat(QAudioFormat::Int16);

	QAudioDevice info(QMediaDevices::defaultAudioOutput());
	if (!info.isFormatSupported(format)) {
		qDebug() << "Default format is not supported";
	}

	sink = new QAudioSink(format, nullptr);
	sink->start(buffer);
}


void AudioRender::handleStateChange(QAudio::State state) {

	switch (state) {
		case QAudio::State::IdleState: {
			//audio data run out;
			qDebug() << "finished rendering audio";
			sink->start(buffer);
			break;
		}
	}
}