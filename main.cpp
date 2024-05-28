#include "UVScenarioEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UVScenarioEditor w;
    w.show();
    return a.exec();
}
