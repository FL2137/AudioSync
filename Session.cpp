#include "Session.hpp"
#include <iostream>

Session::Session(QString _address, int _port) {


	audioC = new AudioHandler(AudioHandler::MODE::CAPTURE);
	audioR = new AudioHandler(AudioHandler::MODE::RENDER);
	audioR->setMutex(&renderMutex);

	renderBuffer = new char[BUFFERSIZE];

	address = _address;
	port = _port;
}

void Session::startSession() {

	server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, port, address);

	audioC->moveToThread(&captureThread);
	audioR->moveToThread(&renderThread);
	audioC->setServer(server);

	connect(this, &Session::runAudioCapture, audioC, &AudioHandler::win32AudioCapture);
	connect(this, &Session::runAudioRender, audioR, &AudioHandler::win32Render);

	captureThread.start();
	renderThread.start();

	emit runAudioCapture();
	emit runAudioRender(renderBuffer);
	
	qDebug() << "Session started properly";
}

Session::~Session() {

	captureThread.quit();
	captureThread.wait();

	renderThread.quit();
	renderThread.wait();

	delete server;

	delete audioCapture;
	delete audioRender;

	delete audioC;
	delete audioR;

	delete[] renderBuffer;
}

void Session::appendTargetEndpoint(QString address, int port) {
	server->addTargetedEndpoint(address, port);
}

void Session::changeRenderVolume(int newVolume) {
	this->audioR->changeVolume(newVolume);
}

void Session::changeCaptureVolume(int newVolume) {
	this->audioC->changeVolume(newVolume);
}