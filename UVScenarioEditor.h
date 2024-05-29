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
private:
    Ui::UVScenarioEditorClass ui;
	ScenarioEditor* m_scenarioEditor = nullptr;
	PrimitiveEditor* m_primitiveEditor = nullptr;

	QString appDir;
	QString scenarioDir;
	QString primitiveDir;

	int timerId;
};
