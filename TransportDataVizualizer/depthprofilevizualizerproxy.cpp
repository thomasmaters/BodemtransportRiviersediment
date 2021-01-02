#include "depthprofilevizualizerproxy.hpp"
#include <iostream>

DepthProfileVizualizerProxy::DepthProfileVizualizerProxy(QWidget* parent) : QWidget(parent)
{
    udp_socket_ = new QUdpSocket(this);
    udp_socket_->bind(QHostAddress::LocalHost, 2000);

    connect(udp_socket_, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

DepthProfileVizualizerProxy::~DepthProfileVizualizerProxy()
{
}

void DepthProfileVizualizerProxy::readPendingDatagrams()
{
    try
    {
        QByteArray datagram;

        datagram.resize(udp_socket_->pendingDatagramSize());
        udp_socket_->readDatagram(datagram.data(), datagram.size());

        Messages::BottomTransportMessage depth_profile(reinterpret_cast<uint8_t*>(datagram.data()));

        messageReceived(depth_profile);
    }
    catch (std::exception& msg)
    {
        QTextStream(stdout) << msg.what() << '\n';
    }
}

void DepthProfileVizualizerProxy::messageReceived(Messages::BottomTransportMessage message)
{
}
