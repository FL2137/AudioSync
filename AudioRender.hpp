#pragma once

#include <qbytearray.h>
#include <qdebug.h>
#include <qobject.h>
#include <qbuffer.h>
#include <qaudiodevice.h>
#include <qaudiosink.h>
#include <qmediadevices.h>


class AudioRender : public QObject {

	Q_OBJECT

public slots:
	void render(QByteArray* buffer);

signals:
	void bufferRendered();

private:
	QAudioSink* sink;
		

	//default format for audio is CD quality (441khz, 16bit sample size, 2 channels) 
};