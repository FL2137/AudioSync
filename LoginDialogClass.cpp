#include "LoginDialogClass.hpp"


LoginDialogClass::LoginDialogClass(Session& session, QWidget* parent) : QDialog(parent) {
	ui.setupUi(&dialog);

	connect(ui.okPush, &QPushButton::clicked, this, [=, &session]() {
		if (ui.loginEdit->text().isEmpty()) {
			ui.loginEdit->setText("Please enter your login");
			return;
		}

		if (ui.passwordEdit->text().isEmpty()) {
			ui.passwordEdit->setText("Please enter your password");
			return;
		}

		TcpRequest request;
		request.type = "LOGIN";
		request.uid = -1;
		json data;

		data["nickname"] = ui.loginEdit->text().toStdString();
		data["password"] = ui.passwordEdit->text().toStdString();

		request.data = data;
		std::string responseStr;
		json response;

		TcpClient::send(request, responseStr);

		if (json::accept(responseStr)) {

			response = json::parse(responseStr);
			if (response["ok"] == "OK") {
				session.uid = response["uid"].get<int>();
				qDebug() << "Received uid: " << session.uid;
				dialog.close();
			}
			else {
				ui.badCredsLabel->setText("Incorrect login or password");
				ui.loginEdit->setText("");
				ui.passwordEdit->setText("");
			}
		}
		else {
			qWarning() << "Invalid response json";
		}

	});
	dialog.show();
	dialog.setFocus();
}
