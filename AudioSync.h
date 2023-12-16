#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AudioSync.h"
#include "AudioHandler.hpp"
#include <qaudiosink.h>
class AudioSync : public QMainWindow
{
    Q_OBJECT

public:
    AudioSync(QWidget *parent = nullptr);
    ~AudioSync();
    QFile tf;
    QAudioSink* audio = nullptr;

public slots:
    void handleStateChange(QAudio::State newState);
    void stopCapture();

private:
    Ui::AudioSyncClass ui;
};
