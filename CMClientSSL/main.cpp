#include "gui.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    w.show();

    return a.exec();
}
