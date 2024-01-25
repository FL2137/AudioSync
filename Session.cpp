#include "Session.hpp"
#include <iostream>

Session::Session(QString _address, int _port) {


	audioCapture = new AudioHandler(AudioHandler::MODE::CAPTURE);
	audioRender = new AudioHandler(AudioHandler::MODE::RENDER);
	audioRender->setMutex(&renderMutex, &serverMutex);
	renderBuffer = new char[BUFFERSIZE];

	address = _address;
	port = _port;
}

void Session::startSession() {

	//server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, port, address);

	//audioCapture->setServer(server);

	//server->moveToThread(&serverThread);
	audioCapture->moveToThread(&captureThread);
	audioRender->moveToThread(&renderThread);
	
	connect(this, &Session::runAudioRender, audioRender, &AudioHandler::win32Render);
	connect(this, &Session::runAudioCapture, audioCapture, &AudioHandler::win32AudioCapture);
	//connect(this, &Session::runServerThread, server, &UdpServer::readPendingData);

	//serverThread.start();
	captureThread.start();
	renderThread.start();

	//emit runServerThread();
	emit runAudioCapture();
	emit runAudioRender(renderBuffer);


	qDebug() << "Session started properly";
}

Session::~Session() {

	captureThread.quit();
	captureThread.wait();

	renderThread.quit();
	renderThread.wait();
	
	serverThread.quit();
	serverThread.wait();

	delete server;

	delete audioCapture;
	delete audioRender;

	delete audioCapture;
	delete audioRender;

	delete[] renderBuffer;
}

void Session::appendTargetEndpoint(QString address, int port) {
	server->addTargetedEndpoint(address, port);
}

void Session::changeRenderVolume(int newVolume) {
	this->audioRender->changeVolume(newVolume);
}

void Session::changeCaptureVolume(int newVolume) {
	this->audioCapture->changeVolume(newVolume);
}
