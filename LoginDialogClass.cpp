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

		SHA256 encryption;

		std::string passHash = ui.passwordEdit->text().toStdString();

		encryption.update(passHash);
		auto digested = encryption.digest();

		qDebug() << SHA256::toString(digested);

		data["nickname"] = ui.loginEdit->text().toStdString();
		data["password"] = SHA256::toString(digested);

		request["data"] = data;

		std::string response;

		BeastClient::syncBeast(request.dump(), response);

		json jsonResponse;

		if (json::accept(response)) {
			jsonResponse = json::parse(response);
		}

		if (jsonResponse["ok"] == "OK") {
			this->uid = jsonResponse["uid"].get<int>();
			emit passUid(this->uid);
			dialog.close();
		}
		else {
			ui.badCredsLabel->setText("Incorrect login or password");
			ui.loginEdit->setText("");
			ui.passwordEdit->setText("");
		}
	});

	dialog.show();
	dialog.setFocus();
}


LoginDialogClass::~LoginDialogClass() {
	delete socket;
}