#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UVScenarioEditor.h"

#include <Qdebug>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <Settings.h>
#include "ScenarioEditor/ScenarioEditor.h"
#include "PrimitiveEditor/PrimitiveEditor.h"
#include "Common/MultipleInputDialog.h"

class UVScenarioEditor : public QMainWindow
{
    Q_OBJECT

public:
    UVScenarioEditor(QWidget *parent = nullptr);
    ~UVScenarioEditor();

public slots:
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionSaveAs_triggered();
	void on_actionPlace_triggered();
	void on_actionPrimitives_triggered();
	void timerEvent(QTimerEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

private:
    Ui::UVScenarioEditorClass ui;
	ScenarioEditor* m_scenarioEditor = nullptr;
	PrimitiveEditor* m_primitiveEditor = nullptr;

	QString appDir;
	QString scenarioDir;
	QString primitiveDir;

	double scale = 1.0;
	QLabel* labelMousePos;

	int timerId;
};
