#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("MarkSentence");
    w.setWindowIcon(QIcon(":/icon/icon/main_title.png"));
    w.show();

    return a.exec();
}
