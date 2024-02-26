#pragma once
#include <qwidget.h>
#include <qstring.h>
#include <string>
#include <qdir.h>
#include <qfile.h>
#include <qevent.h>
#include <qfiledialog.h>

#include "nlohmann/json.hpp"

using nlohmann::json;



class FileDialog : QFileDialog {
	
public:
	FileDialog(QString title, QWidget *parent = nullptr) : QFileDialog(parent) {

		this->setObjectName("fileDialog");
		this->setWindowTitle(title);

		this->show();
		this->setFocus();
	}

	~FileDialog() {
		this->destroy();
	}

signals:

};

class AvatarWidget : QWidget {

public:

	AvatarWidget(QWidget* parent = nullptr) : QWidget(parent) {
		m_parent = parent;

		this->setObjectName("UserAvatar");

		QFile settings(QDir::currentPath() + "/userSettings.json");

		if (!settings.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString path = QDir::currentPath() + "/x64/Debug/Avatars/userAvatar.png";
			this->setStyleSheet("background-image: url(" + path + ")");
		}
		else {
			std::string settingsData = settings.readAll().toStdString();
			json setts;

			if (json::accept(settingsData))
				setts = json::parse(settingsData);
			
			QString path = QDir::currentPath() + "/x64/Debug/Avatars/" + QString::fromStdString(setts["avatar"].get<std::string>());
			this->setStyleSheet("background-image: url(" + path + ")");
		}

		this->setGeometry(QRect(parent->geometry().width() - 210, 30, 100, 100));
	}

	AvatarWidget(QString file, int nthUser, QWidget* parent = nullptr) : QWidget(parent) {
		if (file == "")
			file = "userAvatar";

		this->setObjectName("avatar" + file);
		int n = 1;

		QRect parentGeo = parent->geometry();

		int pY = parentGeo.y();
		int pX = parentGeo.x();

		this->setGeometry(QRect(10 + 100 * nthUser, 10, 100, 100));

		QString avatarPath = QDir::currentPath() + "/x64/Debug/Avatars/" + file + ".png";

		QImage image(avatarPath);

		//scale image if needed 
		if (image.width() != 100 || image.height() != 100) {
			image = image.scaled(100, 100);
			image.save(avatarPath);
		}

		this->setStyleSheet("background-image: url(" + avatarPath + ")");
		this->show();//needed?
	}

	~AvatarWidget() {
		this->destroy(true, true);
	}


private:

	QWidget* m_parent = nullptr;

	FileDialog* dialog = nullptr;



protected:
	
	//onclick implementation
	void mousePressEvent(QMouseEvent *event) override {
		if (event->button() == Qt::MouseButton::LeftButton && parent()->objectName() == "AudioSyncClass") {
			QString filename = QFileDialog::getOpenFileName(m_parent, tr("Open image"), "/", tr("Image (*.png *.jpg *.bmp)"));
			if (!filename.isEmpty()) {
				QFile file(filename, nullptr);
				file.open(QIODevice::ReadOnly);
			
				QString name = filename.right(filename.size() - filename.lastIndexOf("/") - 1);
				json js;

				QFile settings("./userSettings.json");
				settings.open(QIODevice::ReadWrite | QIODevice::Text);
			
				js = json::parse(settings.readAll().toStdString());

				js["avatar"] = name.toStdString();


				settings.resize(0);
				settings.write(js.dump().c_str());

				this->setStyleSheet("background-image: url(" + filename + ")");

				file.close();
				settings.close();
			}
		}
	}
};