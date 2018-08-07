#include "MainApplication.hpp"
#include "ui_mainapplication.h"

MainApplication::MainApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainApplication)
{
    ui->setupUi(this);
}

MainApplication::~MainApplication()
{
    delete ui;
}
