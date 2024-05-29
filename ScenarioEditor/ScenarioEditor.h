#pragma once

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>

#include <QImage>
#include <QPixmap>
#include <QPainter>

#include "Common/Type.h"

class ScenarioEditor  : public QObject
{
	Q_OBJECT

public:
	ScenarioEditor(QObject *parent);
	~ScenarioEditor();

public slots:

public:
	void setFilePath(const QString& filePath);

	void parseFile();
	void saveFile();
	void saveFile(QString path);

	void update();

	void getPixMap(QPixmap& pix);

private:
	QString m_filePath;
	Scenario m_scenario;

	QImage background;
	QImage sceneImg;
	QPixmap pixmap;

	bool changed = false;
};
