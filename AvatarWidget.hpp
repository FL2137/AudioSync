#pragma once
#include <qwidget.h>
#include <qstring.h>
#include <qdir.h>

class AvatarWidget : QWidget {

public:

	AvatarWidget(QString file, QWidget *parent = nullptr) : QWidget(parent) {
		this->setObjectName("avatar"+file);
		int n = 1;

		QRect parentGeo = parent->geometry();

		int pY = parentGeo.y();
		int pX = parentGeo.x();

		this->setGeometry(QRect(10, 10, 100, 100));


		QString avatarPath = QDir::currentPath() + "/x64/Debug/Avatars/" + file + ".png";

		this->setStyleSheet("background-image: url(" + avatarPath + ")");
		this->show();//needed?
	}

	~AvatarWidget() {
		this->destroy(true, true);
	}

};