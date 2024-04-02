#include "WebsocketClient.hpp"

#include <qnetworkrequest.h>

WebsocketClient::WebsocketClient(const QUrl& url, std::function<void(std::string, std::string&)> _parser,  QObject* parent) : QObject(parent) {
	this->parser = _parser;

	QUrl u("ws://192.168.0.109:3005");
	qDebug() << "Websocket client: " << u;
	connect(&socket, &QWebSocket::connected, this, &WebsocketClient::onConnected);
	connect(&socket, &QWebSocket::disconnected, this, &WebsocketClient::closed);

	socket.open(u);

	qDebug() << socket.localAddress() << ":" << socket.localPort();
}

void WebsocketClient::onConnected() {
	qDebug() << "Websocket connected";
	connect(&socket, &QWebSocket::textMessageReceived, this, &WebsocketClient::onTextMessageReceived);

	socket.sendTextMessage("PING");

}

void WebsocketClient::onTextMessageReceived(QString message) {
	if (message == "PONG") return;

	std::string response;
	parser(message.toStdString(), response);
	//optional close and send
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

