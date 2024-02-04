#pragma once

#include "AudioSync.h"
#include "Session.hpp"

#include "ui_LoginDialog.h"
#include <qdialog.h>
#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qlineedit.h>
#include <qcombobox.h>


class LoginDialog : public QDialog {

public:

	LoginDialog(Session& session, QWidget* parent = nullptr) : QDialog(parent) {
		uiLoginDialog.setupUi(this);

		connect(uiLoginDialog.loginPush, &QPushButton::clicked, this, [=, &session]() {
			
			if (uiLoginDialog.loginEdit->text().isEmpty()) {
				uiLoginDialog.loginEdit->setText("Please enter Your login");
				return;
			}
			
			if (uiLoginDialog.passwordEdit->text().isEmpty()) {
				uiLoginDialog.passwordEdit->setText("Please enter Your password");
				return;
			}

			//TODO: check correctness of the credentials

			session.setUserCreds(uiLoginDialog.loginEdit->text(), uiLoginDialog.passwordEdit->text());
			this->close();
		});
	}


private:
	Ui::LoginDialog uiLoginDialog;
};