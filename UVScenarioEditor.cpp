#include "UVScenarioEditor.h"

UVScenarioEditor::UVScenarioEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	appDir = QApplication::applicationDirPath();
	qDebug() << "App dir: " << appDir;

	/// check if directory "Scenarios" exists
	QDir dir(appDir + "/" + SCENARIO_PATH);
	if (!dir.exists())
	{
		qDebug() << "Scenarios directory does not exist";
		/// create directory "Scenarios"
		if (!dir.mkpath("."))
		{
			qDebug() << "Failed to create Scenarios directory";
			QMessageBox::critical(this, "Error", "Failed to create Scenarios directory");
		}
		else
		{
			qDebug() << "Scenarios directory created";
		}
	}
	else
	{
		qDebug() << "Scenarios directory exists";
	}
	scenarioDir = dir.absolutePath();

	/// check if directory "Primitives" exists
	dir = QDir(appDir + "/" + PRIMITIVE_PATH);
	if (!dir.exists())
	{
		qDebug() << "Primitives directory does not exist";
		/// create directory "Primitives"
		if (!dir.mkpath("."))
		{
			qDebug() << "Failed to create Primitives directory";
			QMessageBox::critical(this, "Error", "Failed to create Primitives directory");
		}
		else
		{
			qDebug() << "Primitives directory created";
		}
	}
	else
	{
		qDebug() << "Primitives directory exists";
	}
	primitiveDir = dir.absolutePath();

	/// create ScenarioEditor and PrimitiveEditor
	m_scenarioEditor = new ScenarioEditor(this);
	m_primitiveEditor = new PrimitiveEditor(this);

	timerId = startTimer(10);
}

UVScenarioEditor::~UVScenarioEditor()
{
	delete m_scenarioEditor;
	delete m_primitiveEditor;
}

void UVScenarioEditor::on_actionNew_triggered()
{
    qDebug() << "New action triggered";

	QString scenarioFilePath = QFileDialog::getSaveFileName(this, "New Scenario", scenarioDir, "Scenario Files (*.scn)");
	QFile file(scenarioFilePath);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "Failed to open file: " << scenarioFilePath;
		QMessageBox::critical(this, "Error", "Failed to open file: " + scenarioFilePath);
		return;
	}
	else
	{
		qDebug() << "File opened: " << scenarioFilePath;
	}

	/// dialog to get the scenario size, resolution, etc.
	QStringList labels = {"Width(m)","Height(m)","Resolution(m)"};
	MultipleInputDialog dialog(labels, this);

	QStringList values = dialog.getValues();
	if (!values.isEmpty())
	{
		qDebug() << "Values: " << values;

		QJsonObject scenario;
		scenario["width"] = values[0].toDouble();
		scenario["height"] = values[1].toDouble();
		scenario["resolution"] = values[2].toDouble();

		QJsonDocument doc(scenario);
		file.write(doc.toJson());
		file.close();

		qDebug() << "Scenario file created: " << scenarioFilePath;

		m_scenarioEditor->setFilePath(scenarioFilePath);
	}
	else
	{
		qDebug() << "Dialog cancelled";
		file.close();
		return;
	}
}

void UVScenarioEditor::on_actionOpen_triggered()
{
	qDebug() << "Open action triggered";

	QString scenarioFilePath = QFileDialog::getOpenFileName(this, "Open Scenario", scenarioDir, "Scenario Files (*.scn)");
	QFile file(scenarioFilePath);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "Failed to open file: " << scenarioFilePath;
		QMessageBox::critical(this, "Error", "Failed to open file: " + scenarioFilePath);
		return;
	}
	else
	{
		qDebug() << "File opened: " << scenarioFilePath;
		file.close();
	}

	m_scenarioEditor->setFilePath(scenarioFilePath);
}

void UVScenarioEditor::on_actionSave_triggered()
{
	qDebug() << "Save action triggered";
	m_scenarioEditor->saveFile();
}

void UVScenarioEditor::on_actionSaveAs_triggered()
{
	qDebug() << "Save As action triggered";

	QString scenarioFilePath = QFileDialog::getSaveFileName(this, "Save Scenario As", scenarioDir, "Scenario Files (*.scn)");
	QFile file(scenarioFilePath);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "Failed to open file: " << scenarioFilePath;
		QMessageBox::critical(this, "Error", "Failed to open file: " + scenarioFilePath);
		return;
	}
	else
	{
		qDebug() << "File opened: " << scenarioFilePath;
		file.close();
	}

	m_scenarioEditor->setFilePath(scenarioFilePath);
}

void UVScenarioEditor::on_actionPlace_triggered()
{
	qDebug() << "Place action triggered";
	m_primitiveEditor->show();
}

void UVScenarioEditor::on_actionPrimitives_triggered()
{
	qDebug() << "Primitives action triggered";
}

void UVScenarioEditor::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == timerId)
	{
		m_scenarioEditor->update();
		QPixmap showPix;
		m_scenarioEditor->getPixMap(showPix);
		if (!showPix.isNull())
		{
			showPix = showPix.scaled(ui.labelView->size(), Qt::KeepAspectRatio);
			ui.labelView->setPixmap(showPix);
		}
	}
	else
	{
		QWidget::timerEvent(event);
	}
}
