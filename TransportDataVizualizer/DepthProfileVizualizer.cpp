#include "DepthProfileVizualizer.hpp"
#include <iostream>

DepthProfileVizualizer::DepthProfileVizualizer(QCustomPlot* custom_plot, QCustomPlot* custom_plot2, QWidget* parent)
  : DepthProfileVizualizerProxy(parent),
    custom_plot_(custom_plot),
    custom_plot2_(custom_plot2),
    transport_data_(200),
    transport_key_(200),
    transport_average_(200),
    total_average_(0),
    file("C:/Data.txt")
{
    file.open(QIODevice::ReadWrite);
    custom_plot_->addGraph();
    custom_plot_->xAxis->setRange(0.0, 180.0);

    custom_plot2_->addGraph();
    custom_plot2_->graph(0)->setPen(QPen(Qt::red));

    custom_plot2_->addGraph();
    custom_plot2_->graph(1)->setPen(QPen(Qt::black));
    custom_plot2_->graph(1)->setKeyAxis(custom_plot2_->xAxis);
    custom_plot2_->graph(1)->setValueAxis(custom_plot2_->yAxis);

    custom_plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);
    custom_plot2_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    custom_plot2_->xAxis->setTicker(timeTicker);
}

void DepthProfileVizualizer::messageReceived(Messages::BottomTransportMessage message)
{
    total_average_ += message.getAverageTransport();
    if (transport_data_.size() >= DPV_AVERAGE_LENGTH)
    {
        total_average_ -= transport_data_.at(transport_data_.length() - DPV_AVERAGE_LENGTH);
    }

#ifdef DPV_DEBUG
    std::cout << "Transport: " << message.getAverageTransport() << std::endl;
    std::cout << "Amount dunes: " << std::to_string(message.getAmoundOfDunes()) << std::endl;
    std::cout << "Average: " << total_average_ / DPV_AVERAGE_LENGTH;
#endif
    std::vector<Matrix<7, 1, float>> dune_data = message.getDunes<7>();

    QVector<double> x(message.getAmoundOfDunes() * 25);
    QVector<double> y(message.getAmoundOfDunes() * 25);
    QDateTime ping_time = QDateTime::fromMSecsSinceEpoch(message.getTimeOfPing());

    x.clear();
    y.clear();

    for (Matrix<7, 1, float>& i : dune_data)
    {
        for (std::size_t j = 1; j < 24; ++j)
        {
            double cur_x = (i.at(6, 0) - i.at(5, 0)) / 24 * j;

            x.append(i.at(5, 0) + cur_x);
            y.append(
                -(i.at(0, 0) * std::pow(cur_x, 3) + i.at(1, 0) * std::pow(cur_x, 2) + i.at(2, 0) * cur_x + i.at(3, 0)));
        }
    }
    custom_plot_->graph(0)->setData(x, y);

    transport_key_.push_back(ping_time.toMSecsSinceEpoch());
    transport_data_.push_back(message.getAverageTransport());
    transport_average_.push_back(total_average_ / DPV_AVERAGE_LENGTH);

    custom_plot2_->graph(0)->setData(transport_key_, transport_data_);
    custom_plot2_->graph(1)->addData(ping_time.toMSecsSinceEpoch(), (total_average_ / DPV_AVERAGE_LENGTH));

    QTextStream(stdout) << (total_average_ / DPV_AVERAGE_LENGTH);

    custom_plot2_->xAxis->setRange(ping_time.addSecs(-50).toMSecsSinceEpoch(),
                                   ping_time.addSecs(10).toMSecsSinceEpoch());

    custom_plot_->replot();
    custom_plot2_->replot();
}
