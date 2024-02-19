#pragma once

#include <qdialog.h>
#include <qpushbutton.h>
#include "ui_LoginDialog.h"
#include "TcpClient.hpp"

class LoginDialogClass : public QDialog {

	Q_OBJECT

public:
	LoginDialogClass(QWidget* parent = nullptr);
	int uid;

signals:
	void passUid(int uid);

private:
	Ui::LoginDialog ui;
	QDialog dialog;
};