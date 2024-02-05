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

		TcpClient::send(request, responseStr);
		json response = json::parse(responseStr);

		if (response["type"] == "OK") {
			session.uid = response["uid"].get<int>();
			this->close();
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
