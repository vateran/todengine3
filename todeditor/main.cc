#include "todeditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TodEditor w;
    w.show();

    return a.exec();
}
