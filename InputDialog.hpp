#pragma once

#include "UdpServer.hpp"
#include "AudioSync.h"

#include "ui_ConnectDialog.h"
#include <qdialog.h>
#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qlineedit.h>
#include <qcombobox.h>


class ConnectDialogClass : public QDialog {

public:
	
	ConnectDialogClass(QString& targetAddress, int& targetPort, QWidget* parent = nullptr) : QDialog(parent) {

		ui_conDialog.setupUi(&dialog);

		ui_conDialog.comboBox->addItems(UdpServer::listLocalAddresses());

		connect(ui_conDialog.okButton, &QPushButton::clicked, this, [=, &targetAddress, &targetPort]() {
			targetAddress = ui_conDialog.comboBox->currentText();
			targetPort = ui_conDialog.lineEdit->text().toInt();
			dialog.close();
			this->~ConnectDialogClass();
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