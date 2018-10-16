#ifndef DEPTHPROFILEVIZUALIZER_HPP
#define DEPTHPROFILEVIZUALIZER_HPP

#define DPV_AVERAGE_LENGTH 8

#include "depthprofilevizualizerproxy.hpp"

#include "qcustomplot.h"

#include <QWidget>

class DepthProfileVizualizer : public DepthProfileVizualizerProxy
{
    Q_OBJECT
  public:
    explicit DepthProfileVizualizer(QCustomPlot* custom_plot, QCustomPlot* custom_plot2, QWidget* parent = nullptr);

  protected:
    void messageReceived(Messages::BottomTransportMessage message) override;
  signals:

  private:
    QCustomPlot* custom_plot_;
    QCustomPlot* custom_plot2_;
    QVector<double> transport_data_;
    QVector<double> transport_key_;
    QVector<double> transport_average_;

    double total_average_;

  public slots:
};

#endif  // DEPTHPROFILEVIZUALIZER_HPP
