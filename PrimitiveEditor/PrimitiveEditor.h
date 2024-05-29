#pragma once

#include <QMainWindow>
#include "ui_PrimitiveEditor.h"

#include <Qdebug>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTimerEvent>
#include <Qlistview>
#include <QStringListModel>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>

#include <QImage>
#include <QPixmap>
#include <QPainter>

#include "Settings.h"
#include "Common/MultipleInputDialog.h"
#include "Common/Type.h"

class PrimitiveEditor : public QMainWindow
{
	Q_OBJECT

public:
	PrimitiveEditor(QWidget *parent = nullptr);
	~PrimitiveEditor();

	void parseFile(QString path);
	void saveFile(QString path);
	void refreshView();
	void syncData();

public slots:
	void onCustomContextMenuRequested(const QPoint& pos);
	void onCustomContextMenuRequested_vertex(const QPoint& pos);
	void onCustomContextMenuRequested_anchor(const QPoint& pos);

	void onListViewClicked(const QModelIndex& index);

signals:
	void placePrimitive(Primitive primitive);

private:
	Ui::PrimitiveEditorClass ui;
	QString pathPrifix;

	QStringListModel* model;
	QStringList primitiveNameList;

	QStringListModel* vertexModel;
	QStringList vertexList;

	QStringListModel* anchorModel;
	QStringList anchorList;

	Primitive currenrPrimitive;
};
