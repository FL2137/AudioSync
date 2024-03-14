#include "LoginDialogClass.hpp"

LoginDialogClass::LoginDialogClass(QWidget* parent) : QDialog(parent) {
	ui.setupUi(&dialog);

	connect(ui.okPush, &QPushButton::clicked, this, [=]() {
		if (ui.loginEdit->text().isEmpty()) {
			ui.loginEdit->setText("Please enter your login");
			return;
		}

		if (ui.passwordEdit->text().isEmpty()) {
			ui.passwordEdit->setText("Please enter your password");
			return;
		}


		json request;
		request["type"] = "LOGIN";
		request["uid"] = -1;
		json data;

		data["nickname"] = ui.loginEdit->text().toStdString();
		data["password"] = ui.passwordEdit->text().toStdString();

		request["data"] = data;

		std::string response;

		BeastClient::syncBeast(request.dump(), response);

		qDebug() << response;

		dialog.close();

	});

	dialog.show();
	dialog.setFocus();
}


LoginDialogClass::~LoginDialogClass() {
	delete socket;
}