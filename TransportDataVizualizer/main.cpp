#include <QApplication>
#include "MainApplication.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainApplication w;
    w.show();

    return a.exec();
}
