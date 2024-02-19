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

		TcpRequest request;
		request.type = "LOGIN";
		request.uid = -1;
		json data;

		data["nickname"] = ui.loginEdit->text().toStdString();
		data["password"] = ui.passwordEdit->text().toStdString();

		request.data = data;
		
		json response;

		TcpClient::send(request, response);

		if (response["ok"] == "OK") {
			uid = response["uid"].get<int>();
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
