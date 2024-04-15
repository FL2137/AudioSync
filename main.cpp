#include "AudioSync.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    AudioSync w;
    w.show();
    
    return a.exec();
}
