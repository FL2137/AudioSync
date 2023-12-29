#pragma once

#include <qbytearray.h>
#include <qdebug.h>
#include <qobject.h>
#include <qbuffer.h>
#include <qaudiodevice.h>
#include <qaudiosink.h>
#include <qmediadevices.h>
#include <qfile.h>
#include <qsemaphore.h>

class AudioRender : public QObject {

	Q_OBJECT

public:
	~AudioRender() {
		delete buffer;
		delete sink;
	}

public slots:
	void render(QByteArray* buffer);

signals:
	void bufferRendered();

private:
	void handleStateChange(QAudio::State state);

private:
	QAudioSink* sink;
	QBuffer* buffer;
	QFile* file;
	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 
};