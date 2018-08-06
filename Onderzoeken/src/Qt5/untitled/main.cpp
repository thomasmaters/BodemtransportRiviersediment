#include <QApplication>
#include "onderzoekqtmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OnderzoekQtMain w;
    w.show();

    return a.exec();
}
