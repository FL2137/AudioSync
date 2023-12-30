#include "AudioRender.hpp"

void AudioRender::render(QByteArray *audioArray) {

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
	qDebug() << "RENDERING DATA";
	sink = new QAudioSink(format, nullptr);

	connect(sink, &QAudioSink::stateChanged, this, &AudioRender::handleStateChange);
	for (int i = 0; i < 10; i++) {
		qDebug() << buffer->data()[i];
	}


	mutex->lock();
	qDebug() << "Renderer.Mutex::lock()";
	sink->start(buffer);
	mutex->unlock();
	qDebug() << "Renderer.Mutex::unlock()";

}


void AudioRender::handleStateChange(QAudio::State state) {

	switch (state) {
		case QAudio::State::IdleState: {
			//audio data run out;
			mutex->lock();
			qDebug() << "Renderer.Mutex::lock()";
			buffer->seek(0);
			sink->start(buffer);
			mutex->unlock();
			qDebug() << "Renderer.Mutex::unlock()";
			break;
		}
		default: {
			//lalalal
			break;
		}

	}
}