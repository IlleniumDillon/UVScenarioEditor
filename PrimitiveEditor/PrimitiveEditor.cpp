#include "PrimitiveEditor.h"

PrimitiveEditor::PrimitiveEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	pathPrifix = QApplication::applicationDirPath() + "/" + PRIMITIVE_PATH;

	/// check all files in the directory
	QDir dir(pathPrifix);
	QStringList filters;
	filters << "*.pmt";
	dir.setNameFilters(filters);
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); i++)
	{
		QFileInfo fileInfo = list.at(i);
		primitiveNameList.append(fileInfo.completeBaseName());
	}

	model = new QStringListModel(primitiveNameList, this);
	ui.listView->setModel(model);

	/// add context menu to the list view
	ui.listView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listView, &QListView::customContextMenuRequested, this, &PrimitiveEditor::onCustomContextMenuRequested);

	/// add click event to the list view
	connect(ui.listView, &QListView::clicked, this, &PrimitiveEditor::onListViewClicked);

	/// name line edit
	connect(ui.lineEditName, &QLineEdit::textChanged, this, [=](const QString& text) {
		currenrPrimitive.name = text;
		});
	/// type combo box
	ui.comboBoxType->addItems({ "Polygon", "Circle" });
	connect(ui.comboBoxType, &QComboBox::currentTextChanged, this, [=](const QString& text) {
		currenrPrimitive.type = text;
		refreshView();
		});
	/// vertices table
	vertexModel = new QStringListModel(vertexList, this);
	ui.listViewVertices->setModel(vertexModel);
	ui.listViewVertices->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listViewVertices, &QListView::customContextMenuRequested, this, &PrimitiveEditor::onCustomContextMenuRequested_vertex);

	/// action combo box
	ui.comboBoxActionType->addItems({ "None", "Move", "Rotate" });
	connect(ui.comboBoxActionType, &QComboBox::currentTextChanged, this, [=](const QString& text) {
		currenrPrimitive.action = text;
		refreshView();
		});
	/// anchor table
	anchorModel = new QStringListModel(anchorList, this);
	ui.listViewAnchor->setModel(anchorModel);
	ui.listViewAnchor->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listViewAnchor, &QListView::customContextMenuRequested, this, &PrimitiveEditor::onCustomContextMenuRequested_anchor);
	
	/// save button
	connect(ui.pushButtonSave, &QPushButton::clicked, this, [=]() {
		QModelIndex index = ui.listView->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		QString name = primitiveNameList[index.row()];
		QString path = pathPrifix + "/" + name + ".pmt";
		saveFile(path);
		});
	/// place button
	connect(ui.pushButtonPlace, &QPushButton::clicked, this, [=]() {
		syncData();
		if (currenrPrimitive.name.isEmpty())
		{
			close();
			return;
		}
		emit placePrimitive(currenrPrimitive);
		close();
		});
	/// exit button
	connect(ui.pushButtonExit, &QPushButton::clicked, this, [=]() {
		close();
		});
}

PrimitiveEditor::~PrimitiveEditor()
{

}

void PrimitiveEditor::parseFile(QString path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to open file: " << path;
		return;
	}
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
	if (error.error != QJsonParseError::NoError)
	{
		qDebug() << "Failed to parse JSON: " << error.errorString();
		return;
	}
	QJsonObject obj = doc.object();
	
	QJsonValue name = obj["name"];
	QJsonValue type = obj["type"];
	QJsonValue vertices = obj["vertices"];
	QJsonValue action = obj["action"];
	QJsonValue anchor = obj["anchor"];
	if (name.isNull() || type.isNull() || vertices.isNull() || action.isNull() || (action.toString() != "None" && anchor.isNull()))
	{
		qDebug() << "Invalid JSON format";
		return;
	}
	currenrPrimitive.name = name.toString();
	currenrPrimitive.type = type.toString();
	currenrPrimitive.vertices.clear();
	QJsonArray verticesArray = vertices.toArray();
	for (int i = 0; i < verticesArray.size(); i++)
	{
		QJsonArray vertexObj = verticesArray[i].toArray();
		currenrPrimitive.vertices.append(QPointF(vertexObj[0].toDouble(), vertexObj[1].toDouble()));
	}
	currenrPrimitive.action = action.toString();
	if (action.toString() != "None")
	{
		QJsonArray anchorArray = anchor.toArray();
		for (int i = 0; i < anchorArray.size(); i++)
		{
			QJsonArray anchorObj = anchorArray[i].toArray();
			currenrPrimitive.anchor.append(QPointF(anchorObj[0].toDouble(), anchorObj[1].toDouble()));
		}
	}

	refreshView();
}

void PrimitiveEditor::saveFile(QString path)
{
	/// synchronous the current primitive
	syncData();

	QJsonObject obj;
	obj["name"] = currenrPrimitive.name;
	obj["type"] = currenrPrimitive.type;
	QJsonArray verticesArray;
	for (int i = 0; i < currenrPrimitive.vertices.size(); i++)
	{
		QPointF vertex = currenrPrimitive.vertices[i];
		QJsonArray vertexArray;
		vertexArray.append(vertex.x());
		vertexArray.append(vertex.y());
		verticesArray.append(vertexArray);
	}
	obj["vertices"] = verticesArray;
	obj["action"] = currenrPrimitive.action;
	if (currenrPrimitive.action != "None")
	{
		QJsonArray anchorArray;
		for (int i = 0; i < currenrPrimitive.anchor.size(); i++)
		{
			QPointF anchor = currenrPrimitive.anchor[i];
			QJsonArray anchorArray;
			anchorArray.append(anchor.x());
			anchorArray.append(anchor.y());
			anchorArray.append(anchor.y());
		}
		obj["anchor"] = anchorArray;
	}

	QJsonDocument doc(obj);
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Failed to open file: " << path;
		return;
	}
	file.write(doc.toJson());
	file.close();
}

void PrimitiveEditor::refreshView()
{
	ui.lineEditName->setText(currenrPrimitive.name);
	ui.comboBoxType->setCurrentText(currenrPrimitive.type);
	ui.comboBoxActionType->setCurrentText(currenrPrimitive.action);
	vertexList.clear();
	for (int i = 0; i < currenrPrimitive.vertices.size(); i++)
	{
		vertexList.append(QString::number(currenrPrimitive.vertices[i].x()) + ", " + QString::number(currenrPrimitive.vertices[i].y()));
	}
	vertexModel->setStringList(vertexList);
	anchorList.clear();
	for (int i = 0; i < currenrPrimitive.anchor.size(); i++)
	{
		anchorList.append(QString::number(currenrPrimitive.anchor[i].x()) + ", " + QString::number(currenrPrimitive.anchor[i].y()));
	}
	anchorModel->setStringList(anchorList);
	
	/// get the size of the primitive
	QImage image;
	double resolution = 0.05;
	if (currenrPrimitive.type == "Polygon")
	{
		double minX = std::numeric_limits<double>::max(), minY = std::numeric_limits<double>::max(),
			maxX = std::numeric_limits<double>::min(), maxY = std::numeric_limits<double>::min();
		for (int i = 0; i < currenrPrimitive.vertices.size(); i++)
		{
			QPointF vertex = currenrPrimitive.vertices[i];
			if (vertex.x() < minX)
			{
				minX = vertex.x();
			}
			if (vertex.x() > maxX)
			{
				maxX = vertex.x();
			}
			if (vertex.y() < minY)
			{
				minY = vertex.y();
			}
			if (vertex.y() > maxY)
			{
				maxY = vertex.y();
			}
		}
		double width = maxX - minX;
		double height = maxY - minY;
		resolution = fmax(width, height) / 1024;
		resolution = resolution > 0.05 ? 0.05 : resolution;
		int rows = height / resolution;
		int cols = width / resolution;
		if (rows == 0 || cols == 0)
		{
			return;
		}
		qDebug() << "Width: " << width << ", Height: " << height;
		qDebug() << "Resolution: " << resolution;
		image = QImage(cols, rows, QImage::Format_RGB888);
		image.fill(Qt::white);

		QPoint center(cols / 2, rows / 2);
		qDebug() << "Center: " << center.x() << ", " << center.y();
		QPainter painter(&image);
		painter.setPen(QPen(Qt::black, 1));
		painter.setBrush(Qt::black);

		QPolygon polygon;
		for (int i = 0; i < currenrPrimitive.vertices.size(); i++)
		{
			QPointF vertex = currenrPrimitive.vertices[i];
			int x = vertex.x() / resolution + center.x();
			int y = -vertex.y() / resolution + center.y();
			QPoint point(x, y);
			polygon << point;
		}
		painter.drawPolygon(polygon);
		painter.end();
		/*QPixmap pixmap = QPixmap::fromImage(image);
		pixmap = pixmap.scaled(ui.labelView->size(), Qt::KeepAspectRatio);
		ui.labelView->setPixmap(pixmap);*/
	}
	else if (currenrPrimitive.type == "Circle")
	{
		double radius = sqrt(pow(currenrPrimitive.vertices[0].x(), 2) + pow(currenrPrimitive.vertices[0].y(), 2));
		resolution = radius / 1024;
		resolution = resolution > 0.05 ? 0.05 : resolution;
		int rows = 2 * radius / resolution;
		int cols = 2 * radius / resolution;
		if (rows == 0 || cols == 0)
		{
			return;
		}
		qDebug() << "Radius: " << radius;
		qDebug() << "Resolution: " << resolution;
		image = QImage(cols, rows, QImage::Format_RGB888);
		image.fill(Qt::white);

		QPoint center(cols / 2, rows / 2);
		qDebug() << "Center: " << center.x() << ", " << center.y();
		QPainter painter(&image);
		painter.setPen(QPen(Qt::black, 1));
		painter.setBrush(Qt::black);

		painter.drawEllipse(QPointF(center.x(), center.y()), radius / resolution, radius / resolution);
		painter.end();
	}

	QPainter painter(&image);
	painter.setPen(QPen(Qt::green, 1));
	painter.setBrush(Qt::green);
	painter.drawEllipse(QPoint(image.width() / 2, image.height() / 2), 5, 5);

	painter.setPen(QPen(Qt::red, 1));
	painter.setBrush(Qt::red);
	for (int i = 0; i < currenrPrimitive.vertices.size(); i++)
	{
		QPointF vertex = currenrPrimitive.vertices[i];
		int x = vertex.x() / resolution + image.width() / 2;
		int y = -vertex.y() / resolution + image.height() / 2;
		painter.drawEllipse(QPoint(x, y), 5, 5);
	}

	painter.setPen(QPen(Qt::blue, 1));
	painter.setBrush(Qt::blue);
	for (int i = 0; i < currenrPrimitive.anchor.size(); i++)
	{
		QPointF anchor = currenrPrimitive.anchor[i];
		int x = anchor.x() / resolution + image.width() / 2;
		int y = -anchor.y() / resolution + image.height() / 2;
		painter.drawEllipse(QPoint(x, y), 5, 5);
	}

	painter.end();

	QPixmap pixmap = QPixmap::fromImage(image);
	pixmap = pixmap.scaled(ui.labelView->size(), Qt::KeepAspectRatio);
	ui.labelView->setPixmap(pixmap);
}

void PrimitiveEditor::syncData()
{
	currenrPrimitive.name = ui.lineEditName->text();
	currenrPrimitive.type = ui.comboBoxType->currentText();
	currenrPrimitive.action = ui.comboBoxActionType->currentText();
	currenrPrimitive.vertices.clear();
	for (int i = 0; i < vertexList.size(); i++)
	{
		QStringList vertex = vertexList[i].split(",");
		double x = vertex[0].toDouble();
		double y = vertex[1].toDouble();
		currenrPrimitive.vertices.append(QPointF(x, y));
	}
	currenrPrimitive.anchor.clear();
	for (int i = 0; i < anchorList.size(); i++)
	{
		QStringList anchor = anchorList[i].split(",");
		double x = anchor[0].toDouble();
		double y = anchor[1].toDouble();
		currenrPrimitive.anchor.append(QPointF(x, y));
	}
}

void PrimitiveEditor::onCustomContextMenuRequested_vertex(const QPoint& pos)
{
	/// Add, Delete, Edit
	QMenu* menu = new QMenu(this);
	QAction* action = new QAction("Add", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		MultipleInputDialog* dialog = new MultipleInputDialog(QStringList{ "X", "Y" }, this);
		QStringList values = dialog->getValues();
		if (values.size() == 2)
		{
			double x = values[0].toDouble();
			double y = values[1].toDouble();
			currenrPrimitive.vertices.append(QPointF(x, y));
			vertexList.append(QString::number(x) + ", " + QString::number(y));
			vertexModel->setStringList(vertexList);
		}
		});
	action = new QAction("Delete", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		QModelIndex index = ui.listViewVertices->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		currenrPrimitive.vertices.removeAt(index.row());
		vertexList.removeAt(index.row());
		vertexModel->setStringList(vertexList);
		});
	action = new QAction("Edit", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		QModelIndex index = ui.listViewVertices->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		MultipleInputDialog* dialog = new MultipleInputDialog(QStringList{ "X", "Y" }, this);
		QStringList values = dialog->getValues();
		if (values.size() == 2)
		{
			double x = values[0].toDouble();
			double y = values[1].toDouble();
			currenrPrimitive.vertices[index.row()] = QPointF(x, y);
			vertexList[index.row()] = QString::number(x) + ", " + QString::number(y);
			vertexModel->setStringList(vertexList);
		}
		});
	menu->exec(ui.listViewVertices->mapToGlobal(pos));
	refreshView();
	delete menu;
}

void PrimitiveEditor::onCustomContextMenuRequested_anchor(const QPoint& pos)
{
	/// Add, Delete, Edit
	QMenu* menu = new QMenu(this);
	QAction* action = new QAction("Add", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		MultipleInputDialog* dialog = new MultipleInputDialog(QStringList{ "X", "Y" }, this);
		QStringList values = dialog->getValues();
		if (values.size() == 2)
		{
			double x = values[0].toDouble();
			double y = values[1].toDouble();
			currenrPrimitive.anchor.append(QPointF(x, y));
			anchorList.append(QString::number(x) + ", " + QString::number(y));
			anchorModel->setStringList(anchorList);
		}
		});
	action = new QAction("Delete", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		QModelIndex index = ui.listViewAnchor->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		currenrPrimitive.anchor.removeAt(index.row());
		anchorList.removeAt(index.row());
		anchorModel->setStringList(anchorList);
		});
	action = new QAction("Edit", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		QModelIndex index = ui.listViewAnchor->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		MultipleInputDialog* dialog = new MultipleInputDialog(QStringList{ "X", "Y" }, this);
		QStringList values = dialog->getValues();
		if (values.size() == 2)
		{
			double x = values[0].toDouble();
			double y = values[1].toDouble();
			currenrPrimitive.anchor[index.row()] = QPointF(x, y);
			anchorList[index.row()] = QString::number(x) + ", " + QString::number(y);
			anchorModel->setStringList(anchorList);
		}
		});
	menu->exec(ui.listViewAnchor->mapToGlobal(pos));
	refreshView();
	delete menu;
}

void PrimitiveEditor::onListViewClicked(const QModelIndex& index)
{
	/// load the primitive
	QString name = primitiveNameList[index.row()];
	QString path = pathPrifix + "/" + name + ".pmt";
	parseFile(path);
}

void PrimitiveEditor::onCustomContextMenuRequested(const QPoint& pos)
{
	QMenu* menu = new QMenu(this);
	QAction* action = new QAction("New", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		MultipleInputDialog* dialog = new MultipleInputDialog(QStringList{ "Name" }, this);
		QStringList values = dialog->getValues();
		if (values.size() > 0)
		{
			QString name = values[0];
			QString path = pathPrifix + "/" + name + ".pmt";
			QFile file(path);
			if (file.open(QIODevice::WriteOnly))
			{
				file.close();
				primitiveNameList.append(name);
				model->setStringList(primitiveNameList);
			}
		}
		});
	action = new QAction("Delete", this);
	menu->addAction(action);
	connect(action, &QAction::triggered, this, [=]() {
		QModelIndex index = ui.listView->currentIndex();
		if (!index.isValid())
		{
			return;
		}
		QString name = primitiveNameList[index.row()];
		QString path = pathPrifix + "/" + name + ".pmt";
		QFile file(path);
		if (file.remove())
		{
			primitiveNameList.removeAt(index.row());
			model->setStringList(primitiveNameList);
		}
		});
	menu->exec(ui.listView->mapToGlobal(pos));
	delete menu;
}