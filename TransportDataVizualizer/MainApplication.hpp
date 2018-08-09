#ifndef MAINAPPLICATION_HPP
#define MAINAPPLICATION_HPP

#include <QMainWindow>

namespace Ui
{
class MainApplication;
}

class MainApplication : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainApplication(QWidget *parent = nullptr);
    ~MainApplication();

  private:
    Ui::MainApplication *ui;
};

#endif  // MAINAPPLICATION_HPP
