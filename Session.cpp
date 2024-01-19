#include "Session.hpp"
#include <iostream>

Session::Session(QString _address, int _port) {


	audioC = new AudioHandler(AudioHandler::MODE::CAPTURE);
	audioR = new AudioHandler(AudioHandler::MODE::RENDER);
	audioR->setMutex(&renderMutex, &serverMutex);
	renderBuffer = new char[BUFFERSIZE];

	address = _address;
	port = _port;
}

void Session::startSession() {

	server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, port, address);

	server->moveToThread(&serverThread);
	connect(this, &Session::runServerThread, server, &UdpServer::readPendingData);
	serverThread.start();
	emit runServerThread();

	/*if (audioCapture->initialized) {
		audioCapture->moveToThread(&captureThread);
		audioCapture->setServer(server);
		connect(this, &Session::runAudioCapture, audioCapture, &AudioCapture::win32AudioCapture);
		captureThread.start();
		emit runAudioCapture();
		qDebug() << "Capture works";
	}

	//run render
	if (audioRender->initialized) {
		audioRender->moveToThread(&renderThread);
		connect(this, &Session::runAudioRender, audioRender, &AudioRender::win32Render);
		renderThread.start();
		emit runAudioRender(renderBuffer);
		qDebug() << "Render works";
	}*/

	bool audioDevicesGood = false;

	if (audioC->initialized) {
		audioC->moveToThread(&captureThread);
		audioC->setServer(server);
		connect(this, &Session::runAudioCapture, audioC, &AudioHandler::win32AudioCapture);
		captureThread.start();
		emit runAudioCapture();
		audioDevicesGood = true;
	}


	if (audioR->initialized) {
		audioR->moveToThread(&renderThread);
		connect(this, &Session::runAudioRender, audioR, &AudioHandler::win32Render);
		renderThread.start();
		emit runAudioRender(renderBuffer);
	}
	else {
		audioDevicesGood = false;
	}


	if (audioDevicesGood) {
		qDebug() << "Session started properly";
	}
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
