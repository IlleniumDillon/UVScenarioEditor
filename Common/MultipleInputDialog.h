#pragma once

#include <QObject>
#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QList>

class MultipleInputDialog  : public QObject
{
	Q_OBJECT

public:
	MultipleInputDialog(QStringList lineLabels ,QObject *parent);
	~MultipleInputDialog();

	QStringList getValues();

public:
	QDialog* dialog;
	QList<QLineEdit*> lineEdits;
	QList<QLabel*> labels;
	QVBoxLayout* layout;
	QDialogButtonBox* buttonBox;

	QStringList values;
};
