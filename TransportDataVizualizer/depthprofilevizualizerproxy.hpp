#ifndef DEPTHPROFILEVIZUALIZERPROXY_HPP
#define DEPTHPROFILEVIZUALIZERPROXY_HPP

#include "../SensorInterface/src/BottomTransportMessage.hpp"

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkDatagram>
#include <QTextStream>
#include <QString>

#include <string>

class DepthProfileVizualizerProxy: public QObject
{
    Q_OBJECT
public:
    explicit DepthProfileVizualizerProxy(QObject* parent = 0);

    virtual ~DepthProfileVizualizerProxy();
signals:

public slots:
    void readPendingDatagrams();
private:
    QUdpSocket* udp_socket_;
};

#endif // DEPTHPROFILEVIZUALIZERPROXY_HPP
