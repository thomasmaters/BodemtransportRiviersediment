#include "DepthProfileVizualizer.hpp"
#include <iostream>

DepthProfileVizualizer::DepthProfileVizualizer(QCustomPlot* custom_plot, QWidget* parent)
  : DepthProfileVizualizerProxy(parent), custom_plot_(custom_plot), transport_data_(200), transport_key_(200)
{
    custom_plot_->addGraph();
    custom_plot_->addGraph();
    custom_plot_->xAxis->setRange(0.0, 180.0);
    custom_plot_->yAxis2->setRange(0.0, 5.0);
    custom_plot_->xAxis2->setRange(0.0, 5.0);
    custom_plot_->yAxis2->setVisible(true);
    custom_plot_->xAxis2->setVisible(true);
    custom_plot_->graph(1)->setKeyAxis(custom_plot_->xAxis2);
    custom_plot_->graph(1)->setValueAxis(custom_plot_->yAxis2);

    //    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    //    dateTicker->setDateTimeFormat("yyyy-MM-dd\nhh:mm:ss-zzz");
    //    custom_plot_->xAxis2->setTicker(dateTicker);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    custom_plot_->xAxis2->setTicker(timeTicker);
}

void DepthProfileVizualizer::messageReceived(Messages::BottomTransportMessage message)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << "Transport: " << message.getAverageTransport() << std::endl;
    auto dune_data = message.getDunes<4>();

    QVector<double> x(message.getAmoundOfDunes() * 25);
    QVector<double> y(message.getAmoundOfDunes() * 25);

    x.clear();
    y.clear();

    for (auto& i : dune_data)
    {
        for (std::size_t j = 1; j < 24; ++j)
        {
            double cur_x = (i.at(6, 0) - i.at(5, 0)) / 24 * j;

            x.append(i.at(5, 0) + cur_x);
            y.append(i.at(0, 0) * std::pow(cur_x, 3) + i.at(1, 0) * std::pow(cur_x, 2) + i.at(2, 0) * cur_x +
                     i.at(3, 0));
        }
    }
    std::cout << message.getTimeOfPing() % 1000000 << std::endl;

    //    custom_plot_->yAxis->rescale(true);
    custom_plot_->graph(0)->setData(x, y);
    custom_plot_->graph(1)->setPen(QPen(Qt::red));
    transport_key_.push_back(message.getTimeOfPing() % 1000000);
    transport_data_.push_back(message.getAverageTransport());
    custom_plot_->graph(1)->setData(transport_key_, transport_data_);
    custom_plot_->xAxis2->setRange(transport_key_.last(), 15000, Qt::AlignRight);
    //    custom_plot_->xAxis2->setRange((transport_key_.first()), (transport_key_.last()));

    //    if(transport_data_.size() > 200)
    //    {
    //        transport_key_.erase(transport_key_.begin());
    //        transport_data_.erase(transport_data_.begin());
    //    }
    custom_plot_->graph(1)->addData(custom_plot_->graph(1)->dataCount() + 1, message.getAverageTransport());

    custom_plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);

    custom_plot_->replot();
}
