#include "Session.hpp"
#include <iostream>

Session::Session(int _uid){

	this->uid = _uid;

	audioCapture = new AudioHandler(AudioHandler::MODE::CAPTURE);
	audioRender = new AudioHandler(AudioHandler::MODE::RENDER);

	auto _socket = audioRender->getSocket();
	
	auto endpointData = HolePuncher::punchAhole(_socket); //this lux bad

	json request;

	request["type"] = "SET_ENDPOINT";
	request["uid"] = this->uid;
	request["address"] = std::get<0>(endpointData);
	request["port"] = std::get<1>(endpointData);

	emit sendWebSocketMessage(request.dump());

	renderBuffer = new char[BUFFERSIZE] {0};
}

void Session::startSession() {
	audioRender->printShareCount();

	audioCapture->moveToThread(&captureThread);
	audioRender->moveToThread(&renderThread);
	
	connect(this, &Session::runAudioRender, audioRender, &AudioHandler::win32Render);
	connect(this, &Session::runAudioCapture, audioCapture, &AudioHandler::win32AudioCapture);
	
	captureThread.start();
	renderThread.start();
	
	emit runAudioCapture();
	emit runAudioRender(renderBuffer);

	emit runLoginDialog();

	qDebug() << "Session started properly";
}

Session::~Session() {

	captureThread.quit(); 
	captureThread.wait();

	renderThread.quit();
	renderThread.wait();
	
	delete audioCapture;
	delete audioRender;

	delete[] renderBuffer;
}

void Session::changeRenderVolume(int newVolume) {
	this->audioRender->changeVolume(newVolume);
}

void Session::changeCaptureVolume(int newVolume) {
	this->audioCapture->changeVolume(newVolume);
}

void Session::appendTargetEndpoint(std::string address, int port) {
	audioCapture->appendEndpoint(address, port);
}

void Session::setUserCreds(QString nick) {
	this->nick = nick;
}
