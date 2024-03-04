#include "WebsocketClient.hpp"

WebsocketClient::WebsocketClient(const QUrl& url, QObject* parent) : QObject(parent) {
	qDebug() << "Websocket server: " << url;
	connect(&socket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
	connect(&socket, &QWebSocket::disconnected, this, &WebsocketClient::closed);
	socket.open(url);
}

void WebsocketClient::onConnected() {
	qDebug() << "Websocket connected";
	connect(&socket, &QWebSocket::textMessageReceived, this, &WebsocketClient::onTextMessageReceived);

	//optional send
	//socket.sendTextMessage("lalalalalalal");
}

void WebsocketClient::onTextMessageReceived(QString message) {
	qDebug() << "WebsocketClient received: " << message;

	//optional close
	socket.close();
}