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

		socket = new QWebSocket();


		json request;
		request["type"] = "LOGIN";
		request["uid"] = -1;
		json data;

		data["nickname"] = ui.loginEdit->text().toStdString();
		data["password"] = ui.passwordEdit->text().toStdString();

		request["data"] = data.dump();

		json response;

		connect(socket, &QWebSocket::textMessageReceived, this, [&](const QString& _response) {
			response = json::parse(_response.toStdString());
			if (response["ok"] == "OK") {
				uid = response["uid"].get<int>();
				emit passUid(uid);
				dialog.close();
			}
			else {
				ui.badCredsLabel->setText("Incorrect login or password");
				ui.loginEdit->setText("");
				ui.passwordEdit->setText("");
			}

			socket->close();
			delete socket;
		});
		QUrl url("ws://192.168.1.109:3005");
		socket->open(url);
		qDebug() << "sending: " << QString::fromStdString(request.dump());
		socket->sendTextMessage(QString::fromStdString(request.dump()));
	});
	dialog.show();
	dialog.setFocus();
}
