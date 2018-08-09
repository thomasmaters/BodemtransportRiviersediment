#include "DepthProfileVizualizer.hpp"

DepthProfileVizualizer::DepthProfileVizualizer(QCustomPlot* custom_plot, QWidget* parent)
  : DepthProfileVizualizerProxy(parent), custom_plot_(custom_plot)
{
    custom_plot_->addGraph();
    custom_plot_->addGraph();
    custom_plot_->xAxis->setRange(0.0, 180.0);
}

void DepthProfileVizualizer::messageReceived(Controller::BottomTransportMessage message)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << "Transport: " << message.getAverageTransport() << std::endl;
    auto dune_data = message.getDunes();

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

    custom_plot_->yAxis->rescale(true);
    custom_plot_->graph(0)->setData(x, y);
    custom_plot_->graph(1)->setPen(QPen(Qt::red));
    custom_plot_->graph(1)->addData(custom_plot_->graph(1)->dataCount() + 1, message.getAverageTransport());

    custom_plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems);

    custom_plot_->replot();
}
