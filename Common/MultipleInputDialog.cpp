#include "MultipleInputDialog.h"

MultipleInputDialog::MultipleInputDialog(QStringList lineLabels, QObject* parent = nullptr)
	: QObject(parent)
{
	values.clear();
	dialog = new QDialog();
	layout = new QVBoxLayout();
	dialog->setLayout(layout);

	for (int i = 0; i < lineLabels.size(); i++)
	{
		QHBoxLayout* hlayout = new QHBoxLayout();
		QLabel* label = new QLabel(lineLabels[i]);
		QLineEdit* lineEdit = new QLineEdit();
		hlayout->addWidget(label);
		hlayout->addWidget(lineEdit);
		layout->addLayout(hlayout);
		labels.append(label);
		lineEdits.append(lineEdit);
	}

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	layout->addWidget(buttonBox);

	connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

	if (dialog->exec() == QDialog::Accepted)
	{
		for (int i = 0; i < lineLabels.size(); i++)
		{
			values.append(lineEdits[i]->text());
		}
	}

}

MultipleInputDialog::~MultipleInputDialog()
{
	delete dialog;
}

QStringList MultipleInputDialog::getValues()
{
	return values;
}
