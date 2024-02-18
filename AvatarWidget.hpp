#pragma once
#include <qwidget.h>
#include <qstring.h>
#include <string>
#include <qdir.h>
#include <qfile.h>

#include "nlohmann/json.hpp"

using nlohmann::json;

class AvatarWidget : QWidget {

public:

	AvatarWidget(QWidget* parent = nullptr) : QWidget(parent) {
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

	AvatarWidget(QString file, QWidget* parent = nullptr) : QWidget(parent) {
		this->setObjectName("avatar" + file);
		int n = 1;

		QRect parentGeo = parent->geometry();

		int pY = parentGeo.y();
		int pX = parentGeo.x();

		this->setGeometry(QRect(10, 10, 100, 100));


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

};