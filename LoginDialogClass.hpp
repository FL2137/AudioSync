#pragma once

#include <qdialog.h>
#include <qpushbutton.h>
#include <qwebsocket.h>
#include "ui_LoginDialog.h"
#include "TcpClient.hpp"
#include "SHA256.hpp"
#include "WebsocketClient.hpp"

class LoginDialogClass : public QDialog {

	Q_OBJECT

public:
	LoginDialogClass(QWidget* parent = nullptr);
	~LoginDialogClass();

	int uid = -1;

signals:
	void passUid(int uid);
	void disconnected();

private:
	Ui::LoginDialog ui;
	QDialog dialog;
	QWebSocket* socket = nullptr;
};