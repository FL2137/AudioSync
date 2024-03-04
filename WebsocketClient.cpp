#include "WebsocketClient.hpp"

WebsocketClient::WebsocketClient(const QUrl& url, QObject* parent) : QObject(parent) {
	qDebug() << "Websocket server: " << url;
	connect(&socket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
	connect(&socket, &QWebSocket::disconnected, this, &WebsocketClient::closed);
	socket.open(url);
}


