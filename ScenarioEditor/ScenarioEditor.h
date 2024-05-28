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

private:
	QString m_filePath;

	bool changed = false;
};
