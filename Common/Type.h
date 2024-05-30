#pragma once

#include <QString>
#include <QVector>
#include <QPoint>

class Primitive
{
public:
	Primitive()
	{
		center = QPointF(0, 0);
		rotation = 0;
		vertices.clear();
		action = "None";
		anchor.clear();
	}
	Primitive(QString name, QString type, QPointF center, double rotation, QVector<QPointF> vertices, QString action, QVector<QPointF> anchor)
	{
		this->name = name;
		this->type = type;
		this->center = center;
		this->rotation = rotation;
		this->vertices = vertices;
		this->action = action;
		this->anchor = anchor;
	}
	void updateGeometry()
	{
		if (type == "Polygon")
		{
			verticesTransformed.clear();
			for (int i = 0; i < vertices.size(); i++)
			{
				QPointF vertex = vertices[i];
				double x = vertex.x() * cos(rotation) - vertex.y() * sin(rotation) + center.x();
				double y = vertex.x() * sin(rotation) + vertex.y() * cos(rotation) + center.y();
				verticesTransformed.push_back(QPointF(x, y));
			}
			anchorTransformed.clear();
			for (int i = 0; i < anchor.size(); i++)
			{
				QPointF anchorPoint = anchor[i];
				double x = anchorPoint.x() * cos(rotation) - anchorPoint.y() * sin(rotation) + center.x();
				double y = anchorPoint.x() * sin(rotation) + anchorPoint.y() * cos(rotation) + center.y();
				anchorTransformed.push_back(QPointF(x, y));
			}
		}
	}
	bool contains(QPointF point)
	{
		if (type == "Polygon")
		{
			auto checkLineIntersection = [](QPointF p1, QPointF p2, QPointF p3, QPointF p4) -> bool
				{
					// 快速排斥实验
					if (std::max(p1.x(), p2.x()) < std::min(p3.x(), p4.x()) ||
						std::max(p3.x(), p4.x()) < std::min(p1.x(), p2.x()) ||
						std::max(p1.y(), p2.y()) < std::min(p3.y(), p4.y()) ||
						std::max(p3.y(), p4.y()) < std::min(p1.y(), p2.y()))
					{
						return false;
					}
					// 跨立实验
					double u = (p3.x() - p1.x()) * (p2.y() - p1.y()) - (p2.x() - p1.x()) * (p3.y() - p1.y());
					double v = (p4.x() - p1.x()) * (p2.y() - p1.y()) - (p2.x() - p1.x()) * (p4.y() - p1.y());
					double w = (p1.x() - p3.x()) * (p4.y() - p3.y()) - (p4.x() - p3.x()) * (p1.y() - p3.y());
					double z = (p2.x() - p3.x()) * (p4.y() - p3.y()) - (p4.x() - p3.x()) * (p2.y() - p3.y());
					return u * v <= 0 && w * z <= 0;
				};
			int crossCount = 0;
			for (int i = 0; i < verticesTransformed.size(); i++)
			{
				QPointF p1 = verticesTransformed[i];
				QPointF p2 = verticesTransformed[(i + 1) % verticesTransformed.size()];
				if (checkLineIntersection(point, QPointF(1e6, point.y()), p1, p2))
				{
					crossCount++;
				}
			}
			return crossCount % 2 == 1;
		}
		else if (type == "Circle")
		{
			double dx = point.x() - center.x();
			double dy = point.y() - center.y();
			double distance = sqrt(dx * dx + dy * dy);
			return distance <= sqrt(pow(vertices[0].x(), 2) + pow(vertices[0].y(), 2));
		}
		else
		{
			return false;
		}
	}
public:
	/// common properties
	QString	name;
	QString	type;
	/// geometry properties
	QPointF	center;
	double rotation;
	QVector<QPointF> vertices;
	QVector<QPointF> verticesTransformed;
	/// action properties
	QString	action;
	QVector<QPointF> anchor;
	QVector<QPointF> anchorTransformed;
};

class Scenario
{
public:
	Scenario()
	{
		width = 0;
		height = 0;
		resolution = 0;
	}
	void clear()
	{
		width = 0;
		height = 0;
		resolution = 0;
		primitives.clear();
	}
	void addPrimitive(Primitive primitive)
	{
		primitives.push_back(primitive);
	}
	int findPrimitive(QString name)
	{
		for (int i = 0; i < primitives.size(); i++)
		{
			if (primitives[i].name == name)
			{
				return i;
			}
		}
		return -1;
	}
	void removePrimitive(int index)
	{
		primitives.erase(primitives.begin() + index);
	}
	void removePrimitive(QString name)
	{
		int index = findPrimitive(name);
		if (index != -1)
		{
			removePrimitive(index);
		}
	}
	Primitive& operator[](int index)
	{
		if (index >= 0 && index < primitives.size())
		{
			return primitives[index];
		}
		else
		{
			throw std::out_of_range("Index out of range");

		}
	}
	Primitive& operator[](QString name)
	{
		int index = findPrimitive(name);
		if (index != -1)
		{
			return primitives[index];
		}
		else
		{
			throw std::out_of_range("Primitive not found");
		}
	}
public:
	double width;
	double height;
	double resolution;
	std::vector<Primitive> primitives;
};