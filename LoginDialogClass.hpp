#pragma once

#include <qdialog.h>
#include <qpushbutton.h>
#include "ui_LoginDialog.h"
#include "Session.hpp"
#include "TcpClient.hpp"

class LoginDialogClass : public QDialog {

public:
	LoginDialogClass(Session& session, QWidget* parent = nullptr);

private:
	Ui::LoginDialog ui;
	QDialog dialog;
};