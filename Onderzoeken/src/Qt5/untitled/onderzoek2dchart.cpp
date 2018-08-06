#include "onderzoek2dchart.h"
#include <QtCharts/QtCharts>
#include <chrono>
#include <iostream>
#include <random>

QT_CHARTS_USE_NAMESPACE

Onderzoek2Dchart::Onderzoek2Dchart(QWidget *parent) : QWidget(parent)
{
    this->setUpdatesEnabled(true);

    QBarSet *set0 = new QBarSet("");

    for (std::size_t i = 0; i < 500; ++i)
    {
        *set0 << std::rand() % 5;
    }

    series = new QBarSeries();
    series->append(set0);

    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");

    chart->createDefaultAxes();

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void Onderzoek2Dchart::changeColor()
{
    if (chart->theme() != QChart::ChartTheme::ChartThemeDark)
    {
        chart->setTheme(QChart::ChartTheme::ChartThemeDark);
    }

    setUpdatesEnabled(false);
    auto start         = std::chrono::steady_clock::now();
    QBarSet *set0      = new QBarSet("");
    QList<qreal> *kaas = new QList<qreal>();

    for (std::size_t i = 0; i < 500; ++i)
    {
        kaas->append(std::rand() % 5);
    }

    set0->append(*kaas);

    series->clear();
    series->insert(0, set0);

    setUpdatesEnabled(true);

    auto end  = std::chrono::steady_clock::now();
    auto diff = end - start;
    qInfo() << std::chrono::duration<double, std::milli>(diff).count() << " ms";
}
