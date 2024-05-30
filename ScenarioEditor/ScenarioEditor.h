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
	void forceUpdate();

	void getPixMap(QPixmap& pix);

	void selectPrimitive(int index);
	void selectPrimitive(QPointF position);

	void moveCurrentPrimitive(QPointF position);
	void rotateCurrentPrimitive(qreal angle);

public slots:

	void onAddPrimitive(Primitive primitive);

public:
	QString m_filePath;
	Scenario m_scenario;

	QImage background;
	QImage sceneImg;
	QPixmap pixmap;
	// double scale = 1.0;

	int currentPrimitiveIndex = -1;

	bool changed = false;
};
