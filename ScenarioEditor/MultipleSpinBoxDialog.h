#pragma once

#include <QObject>

#include <Qdebug>
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QTimerEvent>

#include "Common/Type.h"

class MultipleSpinBoxDialog  : public QObject
{
	Q_OBJECT

public:
	MultipleSpinBoxDialog(QObject *parent);
	~MultipleSpinBoxDialog();

public slots:
	void timerEvent(QTimerEvent* event);

private:
	QDialog* dialog;
	QVBoxLayout* layout;
	QHBoxLayout* hlayout;
	QLabel* label;
	QSpinBox* spinBox;
	QDialogButtonBox* buttonBox;
	QPushButton* okButton;
	QPushButton* cancelButton;

	Scenario* scenario;

	int timerId;
};
