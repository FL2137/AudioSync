#pragma once

#include <qwebsocket.h>
#include <qdebug.h>
#include <qobject.h>
#include <qstring.h>
#include <functional>
#include "nlohmann/json.hpp"

using nlohmann::json;

class WebsocketClient : public QObject {
	Q_OBJECT

public:
	WebsocketClient(const QUrl& url, QObject* parent = nullptr); 
	static void makeRequest(QWebSocket *socket, QObject *sender, std::function<void(const QString &response)> handler, const json request);

signals:
	void closed();

private slots:
	void onConnected();
	void onTextMessageReceived(QString message);

private:
	QWebSocket socket;
	bool debug;
};