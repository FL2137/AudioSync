#pragma once

#include "UdpServer.hpp"
#include "AudioSync.h"
#include "Session.hpp"

#include "ui_ConnectDialog.h"
#include <qdialog.h>
#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qlineedit.h>
#include <qcombobox.h>


class ConnectDialogClass : public QDialog {

public:
	
	ConnectDialogClass(Session &session, QWidget* parent = nullptr) : QDialog(parent) {

		ui_conDialog.setupUi(&dialog);

		connect(ui_conDialog.okButton, &QPushButton::clicked, this, [=, &session]() {
			if (!ui_conDialog.addressLine->text().isEmpty() && !ui_conDialog.portLine->text().isEmpty()) {
				session.appendTargetEndpoint(ui_conDialog.addressLine->text(), ui_conDialog.portLine->text().toInt());
				dialog.close();
				this->~ConnectDialogClass();
			}
		});

		dialog.show();
		dialog.setFocus();
	}

	~ConnectDialogClass() {
		delete combo;
		delete lineEdit;
	}

	QComboBox* combo = nullptr;
	QLineEdit* lineEdit = nullptr;

private:
	Ui::ConnectDialog ui_conDialog;
	QDialog dialog;

	QList<QString> addressList = {};

};