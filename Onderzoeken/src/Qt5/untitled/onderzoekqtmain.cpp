#include "onderzoekqtmain.h"
#include "onderzoek2dchart.h"
#include "ui_onderzoekqtmain.h"
#include <QtDataVisualization/QtDataVisualization>

OnderzoekQtMain::OnderzoekQtMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OnderzoekQtMain)
{

    Onderzoek2Dchart* chart = new Onderzoek2Dchart();
    ui->setupUi(this);
        connect(ui->pushButton, SIGNAL (released()),chart, SLOT (changeColor()));
    ui->horizontalLayout->addWidget(chart->chartView);
}

OnderzoekQtMain::~OnderzoekQtMain()
{
    delete ui;
}
