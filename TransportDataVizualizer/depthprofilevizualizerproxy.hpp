#ifndef DEPTHPROFILEVIZUALIZERPROXY_HPP
#define DEPTHPROFILEVIZUALIZERPROXY_HPP

#include "../SensorInterface/src/BottomTransportMessage.hpp"

#include <QString>
#include <QTextStream>
#include <QtNetwork/QNetworkDatagram>
#include <QtNetwork/QUdpSocket>
#include <QtWidgets/QtWidgets>

#include <string>

class DepthProfileVizualizerProxy : public QWidget
{
    Q_OBJECT
  public:
    explicit DepthProfileVizualizerProxy(QWidget* parent = 0);

    virtual ~DepthProfileVizualizerProxy();

  protected:
    virtual void messageReceived(Controller::BottomTransportMessage message);
  signals:

  public slots:
    void readPendingDatagrams();

  private:
    QUdpSocket* udp_socket_;
};

#endif  // DEPTHPROFILEVIZUALIZERPROXY_HPP
