#include "Session.hpp"



void Session::startSession() {

	server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, port, address);

	audioCapture->moveToThread(&captureThread);
	audioRender->moveToThread(&renderThread);

	captureThread.start();
	renderThread.start();

	emit audioRender->win32Render(renderBuffer);
	emit audioCapture->win32AudioCapture();

	qDebug() << "session started";
}


Session::~Session() {

	captureThread.quit();
	captureThread.wait();

	renderThread.quit();
	renderThread.wait();

	delete server;

	delete audioCapture;
	delete audioRender;

	delete[] renderBuffer;
}

void Session::appendTargetEndpoint(QString address, int port) {
	server.addTargetedAddress(address, int port);
}