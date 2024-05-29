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
	QJsonObject obj = doc.object();
	QJsonValue width = obj["width"];
	QJsonValue height = obj["height"];
	QJsonValue resolution = obj["resolution"];
	QJsonValue primitives = obj["primitives"];
	if (width.isNull() || height.isNull() || resolution.isNull() || primitives.isUndefined())
	{
		qDebug() << "Invalid JSON format";
		QMessageBox::critical(nullptr, "Error", "Invalid JSON format");
		return;
	}

	m_scenario.clear();
	m_scenario.width = width.toDouble();
	m_scenario.height = height.toDouble();
	m_scenario.resolution = resolution.toDouble();

	QJsonArray primitiveArray = primitives.toArray();
	for (int i = 0; i < primitiveArray.size(); i++)
	{
		QJsonObject primitiveObj = primitiveArray[i].toObject();
		Primitive primitive;
		QJsonValue name = primitiveObj["name"];
		QJsonValue type = primitiveObj["type"];
		QJsonValue center = primitiveObj["center"];
		QJsonValue rotation = primitiveObj["rotation"];
		QJsonValue vertices = primitiveObj["vertices"];
		QJsonValue action = primitiveObj["action"];
		QJsonValue anchor = primitiveObj["anchor"];
		if (name.isNull() || 
			type.isNull() ||
			center.isNull() ||
			rotation.isNull() ||
			vertices.isNull() ||
			action.isNull() ||
			(action.toString() != "None" && anchor.isNull()))
		{
			qDebug() << "Invalid JSON format";
			QMessageBox::critical(nullptr, "Error", "Invalid JSON format");
			return;
		}

		primitive.name = name.toString();
		primitive.type = type.toString();
		QJsonArray centerArray = center.toArray();
		primitive.center = QPointF(centerArray[0].toDouble(), centerArray[1].toDouble());
		primitive.rotation = rotation.toDouble();
		QJsonArray verticesArray = vertices.toArray();
		for (int j = 0; j < verticesArray.size(); j++)
		{
			QJsonArray vertexArray = verticesArray[j].toArray();
			primitive.vertices.push_back(QPointF(vertexArray[0].toDouble(), vertexArray[1].toDouble()));
		}
		primitive.action = action.toString();
		if (primitive.action != "None")
		{
			QJsonArray anchorArray = anchor.toArray();
			for (int j = 0; j < anchorArray.size(); j++)
			{
				QJsonArray anchorPointArray = anchorArray[j].toArray();
				primitive.anchor.push_back(QPointF(anchorPointArray[0].toDouble(), anchorPointArray[1].toDouble()));
			}
		}
	}

	/// generate background image
	background = QImage(m_scenario.width / m_scenario.resolution, m_scenario.height / m_scenario.resolution, QImage::Format_RGB888);
	background.fill(Qt::gray);
	changed = true;
	update();
}

void ScenarioEditor::saveFile()
{
	saveFile(m_filePath);
}

void ScenarioEditor::saveFile(QString path)
{
	QJsonObject scenario;
	scenario["width"] = m_scenario.width;
	scenario["height"] = m_scenario.height;
	scenario["resolution"] = m_scenario.resolution;

	QJsonArray primitives;
	for (int i = 0; i < m_scenario.primitives.size(); i++)
	{
		Primitive primitive = m_scenario.primitives[i];
		QJsonObject primitiveObj;
		primitiveObj["name"] = primitive.name;
		primitiveObj["type"] = primitive.type;
		QJsonArray center;
		center.push_back(primitive.center.x());
		center.push_back(primitive.center.y());
		primitiveObj["center"] = center;
		primitiveObj["rotation"] = primitive.rotation;
		QJsonArray vertices;
		for (int j = 0; j < primitive.vertices.size(); j++)
		{
			QJsonArray vertex;
			vertex.push_back(primitive.vertices[j].x());
			vertex.push_back(primitive.vertices[j].y());
			vertices.push_back(vertex);
		}
		primitiveObj["vertices"] = vertices;
		primitiveObj["action"] = primitive.action;
		if (primitive.action != "None")
		{
			QJsonArray anchor;
			for (int j = 0; j < primitive.anchor.size(); j++)
			{
				QJsonArray anchorPoint;
				anchorPoint.push_back(primitive.anchor[j].x());
				anchorPoint.push_back(primitive.anchor[j].y());
				anchor.push_back(anchorPoint);
			}
			primitiveObj["anchor"] = anchor;
		}
		primitives.push_back(primitiveObj);
	}
	scenario["primitives"] = primitives;

	QJsonDocument doc(scenario);
	QFile file(path);
	if (!file.open(QIODevice::ReadWrite))
	{
		qDebug() << "Failed to open file: " << path;
		QMessageBox::critical(nullptr, "Error", "Failed to open file: " + path);
		return;
	}
	file.write(doc.toJson());
	file.close();

	qDebug() << "Scenario file saved: " << path;
}

void ScenarioEditor::update()
{
	if (!changed)
	{
		return;
	}

	sceneImg = QImage(m_scenario.width / m_scenario.resolution, m_scenario.height / m_scenario.resolution, QImage::Format_RGB888);
	sceneImg.fill(Qt::white);

	for (int i = 0; i < m_scenario.primitives.size(); i++)
	{
		m_scenario.primitives[i].updateGeometry();
		if (m_scenario.primitives[i].type == "Polygon")
		{
			QPolygon polygon;
			for (int j = 0; j < m_scenario.primitives[i].verticesTransformed.size(); j++)
			{
				QPoint point(m_scenario.primitives[i].verticesTransformed[j].x() / m_scenario.resolution, m_scenario.primitives[i].verticesTransformed[j].y() / m_scenario.resolution);
				polygon << point;
			}
			QPainter painter(&sceneImg);
			painter.setPen(QPen(Qt::black, 1));
			painter.setBrush(Qt::black);
			painter.drawPolygon(polygon);
		}
		else if (m_scenario.primitives[i].type == "Circle")
		{
			QPainter painter(&sceneImg);
			painter.setPen(QPen(Qt::black, 1));
			painter.setBrush(Qt::black);
			painter.drawEllipse(QPointF(m_scenario.primitives[i].center.x() / m_scenario.resolution, m_scenario.primitives[i].center.y() / m_scenario.resolution), m_scenario.primitives[i].vertices[0].x() / m_scenario.resolution, m_scenario.primitives[i].vertices[0].y() / m_scenario.resolution);
		}
	}

	pixmap = QPixmap::fromImage(sceneImg);
	
	changed = false;
}

void ScenarioEditor::getPixMap(QPixmap& pix)
{
	pix = pixmap;
}
