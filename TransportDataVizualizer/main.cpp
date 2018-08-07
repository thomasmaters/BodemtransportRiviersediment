#include "MainApplication.hpp"
#include "depthprofilevizualizerproxy.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainApplication w;
    DepthProfileVizualizerProxy* b = new DepthProfileVizualizerProxy;
    w.show();

    return a.exec();
}
