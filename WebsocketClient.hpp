#pragma once

#include <qwebsocket.h>
#include <qdebug.h>
#include <qobject.h>
#include <qstring.h>


class WebsocketClient : public QObject {
	Q_OBJECT

public:
	WebsocketClient(const QUrl& url, QObject* parent = nullptr);
			

signals:
	void closed();

private slots:
	void onConnected();
	void onTextMessageReceived(QString message);

private:
	QWebSocket socket;
	bool debug;
};