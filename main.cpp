#include "AudioSync.h"
#include "AudioHandler.hpp"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    CoInitializeEx(0, COINIT_MULTITHREADED);
    const int bufsize = 1000000000;

    uint16_t* buffer = new uint16_t[bufsize]{ 0 };
    AudioHandler ah;
    int a;
    ah.winAudioCapture(buffer, bufsize);
    std::cout << "mimi\n";
    std::cin >> a;
    ah.winAudioRender(buffer, bufsize);



    /*QApplication a(argc, argv);
    AudioSync w;
    w.show();

    return a.exec();*/
    CoUninitialize();
    return 0;
}
