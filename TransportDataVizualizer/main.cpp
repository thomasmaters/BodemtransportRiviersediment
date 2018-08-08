#include "MainApplication.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainApplication w;
//    DepthProfileVizualizerProxy b;
    w.show();

    return a.exec();
}
