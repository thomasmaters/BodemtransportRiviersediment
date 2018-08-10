#ifndef DEPTHPROFILEVIZUALIZER_HPP
#define DEPTHPROFILEVIZUALIZER_HPP

#include "depthprofilevizualizerproxy.hpp"

#include "qcustomplot.h"

#include <QWidget>

class DepthProfileVizualizer : public DepthProfileVizualizerProxy
{
    Q_OBJECT
  public:
    explicit DepthProfileVizualizer(QCustomPlot* custom_plot, QWidget* parent = nullptr);

  protected:
    void messageReceived(Controller::BottomTransportMessage message) override;
  signals:

  private:
    QCustomPlot* custom_plot_;
    QVector<double> transport_data_;
    QVector<double> transport_key_;

  public slots:
};

#endif  // DEPTHPROFILEVIZUALIZER_HPP
