#include "ScenarioEditor.h"

ScenarioEditor::ScenarioEditor(QObject *parent)
	: QObject(parent)
{}

ScenarioEditor::~ScenarioEditor()
{}

void ScenarioEditor::setFilePath(const QString& filePath)
{
	m_filePath = filePath;
	parseFile();
}

void ScenarioEditor::parseFile()
{
	QFile file(m_filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to open file: " << m_filePath;
		QMessageBox::critical(nullptr, "Error", "Failed to open file: " + m_filePath);
		return;
	}

	QTextStream in(&file);
	QString fileContent = in.readAll();
	file.close();

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(fileContent.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError)
	{
		qDebug() << "Failed to parse JSON: " << error.errorString();
		QMessageBox::critical(nullptr, "Error", "Failed to parse JSON: " + error.errorString());
		return;
	}
	/// Parse JSON ,generate object list ,generate scene
	changed = true;
}

void ScenarioEditor::saveFile()
{
	saveFile(m_filePath);
}

void ScenarioEditor::saveFile(QString path)
{
}

void ScenarioEditor::update()
{
	if (!changed)
	{
		return;
	}
	changed = false;
}
