#include "MainApplication.hpp"
#include "ui_mainapplication.h"

#include "depthprofilevizualizerproxy.hpp"
#include "DepthProfileVizualizer.hpp"

MainApplication::MainApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainApplication)
{

    ui->setupUi(this);
    DepthProfileVizualizer* c = new DepthProfileVizualizer(ui->customplot);
}

MainApplication::~MainApplication()
{
    delete ui;
}
