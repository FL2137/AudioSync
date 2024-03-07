#include "WebsocketClient.hpp"

#include <qnetworkrequest.h>

WebsocketClient::WebsocketClient(const QUrl& url, QObject* parent) : QObject(parent) {
	QUrl u("ws://192.168.1.109:3005");
	qDebug() << "Websocket client: " << u;
	connect(&socket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
	connect(&socket, &QWebSocket::disconnected, this, &WebsocketClient::closed);

	socket.open(u);
}

void WebsocketClient::onConnected() {
	qDebug() << "Websocket connected";
	connect(&socket, &QWebSocket::textMessageReceived, this, &WebsocketClient::onTextMessageReceived);

	socket.sendTextMessage("lalalalalalal");
}

void WebsocketClient::onTextMessageReceived(QString message) {
	qDebug() << "WebsocketClient received: " << message;
	static int i = 1;
	QString str = message + QString::fromStdString(std::to_string(i));
	socket.sendTextMessage(str);
	//optional close
}