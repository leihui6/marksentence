#include "mainwindow.h"
#include "mydialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("MarkSentence");
    w.show();

    return a.exec();
}
