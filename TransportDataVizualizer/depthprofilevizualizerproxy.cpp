#include "depthprofilevizualizerproxy.hpp"

DepthProfileVizualizerProxy::DepthProfileVizualizerProxy(QObject* parent): QObject(parent)
{
    udp_socket_ = new QUdpSocket(this);
    udp_socket_->bind(QHostAddress::LocalHost, 2000);

    connect(udp_socket_, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

DepthProfileVizualizerProxy::~DepthProfileVizualizerProxy()
    {

    }

void DepthProfileVizualizerProxy::readPendingDatagrams()
{
    while (udp_socket_->hasPendingDatagrams()) {
        QTextStream(stdout) << "Got packat";
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        QTextStream(stdout) << datagram.data().size();
        SensorMessage message = SensorMessage((uint8_t*)datagram.data().data(),2048);
        Controller::BottomTransportMessage depth_profile(message);
        for(auto& i: depth_profile.getDunes())
        {
            QTextStream(stdout) << QString::fromStdString(i.to_string());
        }

        datagram.data().data();
    }
}
