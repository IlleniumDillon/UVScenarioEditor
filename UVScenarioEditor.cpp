#include "UVScenarioEditor.h"

UVScenarioEditor::UVScenarioEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	setMouseTracking(true);
	this->centralWidget()->setMouseTracking(true);
	ui.labelView->setMouseTracking(true);

	labelMousePos = new QLabel(this);
	ui.statusBar->addWidget(labelMousePos);

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

	/// connect signals and slots
	connect(m_primitiveEditor, SIGNAL(placePrimitive(Primitive)), m_scenarioEditor, SLOT(onAddPrimitive(Primitive)));
	

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
			scale = showPix.width() / m_scenarioEditor->m_scenario.width;
			ui.labelView->setPixmap(showPix);
		}
	}
	else
	{
		QWidget::timerEvent(event);
	}
}

void UVScenarioEditor::mousePressEvent(QMouseEvent* event)
{
	QPoint pos = event->pos();
	QPoint centralPos = this->centralWidget()->pos();
	QPoint viewPos = ui.labelView->pos();
	QPoint scenePos = pos - centralPos - viewPos;
	QPointF origin(-m_scenarioEditor->m_scenario.width / 2, -m_scenarioEditor->m_scenario.height / 2);
	QPointF position(scenePos.x() / scale + origin.x(), -scenePos.y() / scale - origin.y());

	m_scenarioEditor->selectPrimitive(position);
	m_scenarioEditor->forceUpdate();
	qDebug() << "Current Primitive: " << m_scenarioEditor->currentPrimitiveIndex;
}

void UVScenarioEditor::mouseReleaseEvent(QMouseEvent* event)
{
	m_scenarioEditor->currentPrimitiveIndex = -1;
	m_scenarioEditor->forceUpdate();
	qDebug() << "Release";
}

void UVScenarioEditor::mouseMoveEvent(QMouseEvent* event)
{
	/// show the current mouse position in the status bar
	QPoint pos = event->pos();
	QPoint centralPos = this->centralWidget()->pos();
	QPoint viewPos = ui.labelView->pos();
	QPoint scenePos = pos - centralPos - viewPos;
	QPointF origin(-m_scenarioEditor->m_scenario.width / 2, -m_scenarioEditor->m_scenario.height / 2);
	QPointF position(scenePos.x() / scale + origin.x(), -scenePos.y() / scale - origin.y());
	// QString str = QString("Mouse Pos: (%1, %2)").arg(scenePos.x()).arg(scenePos.y());
	// if (m_scenarioEditor->m_scenario.resolution)
	// double resolution = m_scenarioEditor->m_scenario.resolution / scale;
	QString str = QString("Mouse Pos: (%1, %2)").arg(position.x()).arg(position.y());
	labelMousePos->setText(str);

	if (m_scenarioEditor->currentPrimitiveIndex != -1)
	{
		m_scenarioEditor->moveCurrentPrimitive(position);
	}
}

void UVScenarioEditor::wheelEvent(QWheelEvent* event)
{
	if (m_scenarioEditor->currentPrimitiveIndex != -1)
	{
		int delta = event->angleDelta().y();
		qDebug() << "Wheel Delta: " << delta;
		qreal angle = m_scenarioEditor->m_scenario.primitives[m_scenarioEditor->currentPrimitiveIndex].rotation;
		angle += delta / 12.0 / 360.0 * 2.0 * 3.1415926;
		m_scenarioEditor->rotateCurrentPrimitive(angle);
	}
}

void UVScenarioEditor::mouseDoubleClickEvent(QMouseEvent* event)
{
	qDebug() << "Mouse Double Clicked";
}

void UVScenarioEditor::keyPressEvent(QKeyEvent* event)
{
	/// delete the current primitive
	if (event->key() == Qt::Key_Delete)
	{
		if (m_scenarioEditor->currentPrimitiveIndex != -1)
		{
			m_scenarioEditor->m_scenario.removePrimitive(m_scenarioEditor->currentPrimitiveIndex);
			m_scenarioEditor->currentPrimitiveIndex = -1;
			m_scenarioEditor->forceUpdate();
		}
	}
}
