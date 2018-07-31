#include "onderzoekqtmain.h"
#include "onderzoek2dchart.h"
#include "onderzoek3dchart.h"
#include "ui_onderzoekqtmain.h"
#include <QtDataVisualization/QtDataVisualization>

OnderzoekQtMain::OnderzoekQtMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OnderzoekQtMain)
{

    Onderzoek2Dchart* chart = new Onderzoek2Dchart();
    Onderzoek3Dchart* kaas = new Onderzoek3Dchart();
    ui->setupUi(this);
        connect(ui->pushButton, SIGNAL (released()),chart, SLOT (changeColor()));
    ui->horizontalLayout->addWidget(kaas->container);
    ui->horizontalLayout_4->addWidget(chart->chartView);
}

OnderzoekQtMain::~OnderzoekQtMain()
{
    delete ui;
}
