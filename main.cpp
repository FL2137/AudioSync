#include "AudioSync.h"
#include "AudioHandler.hpp"
#include <QtWidgets/QApplication>
#include "HolePuncher.hpp"





int main(int argc, char *argv[]) {







    QApplication a(argc, argv);
    AudioSync w;
    w.show();

    return a.exec();
}
