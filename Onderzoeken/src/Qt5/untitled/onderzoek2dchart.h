#ifndef ONDERZOEK2DCHART_H
#define ONDERZOEK2DCHART_H

#include <QWidget>
#include <QtCharts>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>

class Onderzoek2Dchart : public QWidget
{
    Q_OBJECT
public:
    explicit Onderzoek2Dchart(QWidget *parent = nullptr);
     QChart *chart;
     QChartView *chartView;
     QBarSeries *series;

signals:

public slots:
    void changeColor();
};

#endif // ONDERZOEK2DCHART_H
