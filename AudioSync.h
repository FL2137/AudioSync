#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AudioSync.h"

class AudioSync : public QMainWindow
{
    Q_OBJECT

public:
    AudioSync(QWidget *parent = nullptr);
    ~AudioSync();

private:
    Ui::AudioSyncClass ui;
};
