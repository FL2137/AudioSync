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

	socket.sendTextMessage(QString("lalalala"));
}

void WebsocketClient::onTextMessageReceived(QString message) {
	qDebug() << "WebsocketClient received: " << message;
	
	//optional close
}

void WebsocketClient::makeRequest(QWebSocket* socket, QObject *sender, std::function<void(const QString& response)> *handler, const json request) {
	QUrl u("ws://192.168.1.109:3005");

	connect(socket, &QWebSocket::disconnected, socket, []() {});

	connect(socket, &QWebSocket::connected, sender, [&]() {
		
		connect(socket, &QWebSocket::textMessageReceived, sender, [&](const QString& message) {
			//handler(message);
			socket->close();
		});

		socket->sendTextMessage(QString::fromStdString(request.dump()));
	});
	
	
	socket->open(u);
}

