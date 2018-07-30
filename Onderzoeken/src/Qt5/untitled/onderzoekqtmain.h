#ifndef ONDERZOEKQTMAIN_H
#define ONDERZOEKQTMAIN_H

#include <QMainWindow>

namespace Ui {
class OnderzoekQtMain;
}

class OnderzoekQtMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit OnderzoekQtMain(QWidget *parent = nullptr);
    ~OnderzoekQtMain();

private:
    Ui::OnderzoekQtMain *ui;
};

#endif // ONDERZOEKQTMAIN_H
