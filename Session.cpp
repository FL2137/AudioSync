#include "Session.hpp"



void Session::startSession() {

	server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, port, address);

	audioCapture->moveToThread(&captureThread);
	audioRender->moveToThread(&renderThread);
	audioCapture->setServer(server);

	connect(this, &Session::runAudioCapture, audioCapture, &AudioCapture::win32AudioCapture);
	connect(this, &Session::runAudioRender, audioRender, &AudioRender::win32Render);

	captureThread.start();
	renderThread.start();

	//emit audioCapture->win32AudioCapture();
	emit runAudioCapture();
	emit runAudioRender(renderBuffer);

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
	server->addTargetedEndpoint(address, port);
}