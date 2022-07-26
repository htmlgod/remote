#include "remote_management.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    remote_management w;
    w.show();
    return a.exec();
}


