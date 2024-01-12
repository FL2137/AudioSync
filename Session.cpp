#include "Session.hpp"



void Session::startSession() {
	
	audioCapture->moveToThread(&captureThread);
	audioRender->moveToThread(&renderThread);

	captureThread.start();
	renderThread.start();

	emit audioRender->win32Render(renderBuffer);
	emit audioCapture->win32AudioCapture();


		server = new UdpServer(renderBuffer, &renderMutex, &serverMutex, ui.portLineEdit->text().toShort(), localAddress);
		server->setSemaphores(renderer->renderSem, renderer->serverSem);
		capturer->setServer(server);
		server->readPendingData();

		ui.connectButton->setEnabled(false);
		});



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
