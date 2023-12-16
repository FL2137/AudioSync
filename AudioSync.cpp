#include "AudioSync.h"

AudioSync::AudioSync(QWidget *parent)
    : QMainWindow(parent)
{
   /* QByteArray qba;
    int iter = 10000000;
    qba.reserve(iter);

    AudioHandler as;
    as.winAudioCapture(buffer, iter);

    QBuffer buf;
    QByteArray ar;
    ar.resize(iter);
    for(int i = 0; )*/
   




    ui.setupUi(this);
}


void AudioSync::stopCapture() {
    audio->stop();
    tf.close();
    delete audio;
}

void AudioSync::handleStateChange(QAudio::State newState) {
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        AudioSync::stopCapture();
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (audio->error() != QAudio::NoError) {
            // Error handling
        }
        break;

    default:
        // ... other cases as appropriate
        break;
    }
}


AudioSync::~AudioSync()
{}
