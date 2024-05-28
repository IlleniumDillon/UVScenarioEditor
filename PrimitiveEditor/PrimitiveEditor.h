#pragma once

#include <QObject>

class PrimitiveEditor  : public QObject
{
	Q_OBJECT

public:
	PrimitiveEditor(QObject *parent);
	~PrimitiveEditor();
};
