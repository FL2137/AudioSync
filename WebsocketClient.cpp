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

	json js;
	json data;
	data["nickname"] = "srarara";
	data["password"] = "ajjmw";

	js["type"] = "LOGIN";
	js["data"] = data;

	socket.sendTextMessage(QString::fromStdString(js.dump()));
}

void WebsocketClient::onTextMessageReceived(QString message) {
	qDebug() << "WebsocketClient received: " << message;
	//optional close
}

void WebsocketClient::makeRequest(json request, QString &response) {
	QUrl u("ws://192.168.1.109:3005");
	QWebSocket _sock;
	_sock.open(u);

	_sock.sendTextMessage(QString::fromStdString(request.dump()));

	_sock.textMessageReceived(response);

}