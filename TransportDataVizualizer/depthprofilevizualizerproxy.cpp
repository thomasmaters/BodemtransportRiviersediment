#include "depthprofilevizualizerproxy.hpp"

DepthProfileVizualizerProxy::DepthProfileVizualizerProxy(QWidget* parent): QWidget (parent)
{
    udp_socket_ = new QUdpSocket(this);
    udp_socket_->bind(QHostAddress::LocalHost, 2000);

    connect(udp_socket_, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
}

DepthProfileVizualizerProxy::~DepthProfileVizualizerProxy()
{
    QTextStream(stdout) << __PRETTY_FUNCTION__;
}

void DepthProfileVizualizerProxy::readPendingDatagrams()
{
    try {
        QTextStream(stdout) << "Got packet";
            QByteArray datagram;

            datagram.resize(udp_socket_->pendingDatagramSize());
            udp_socket_->readDatagram(datagram.data(), datagram.size());

            QTextStream(stdout) << "\nConstructed SensorMessage: " << datagram.size() << "\n";
            Controller::BottomTransportMessage depth_profile(reinterpret_cast<uint8_t*>(datagram.data()));

            messageReceived(depth_profile);
    } catch (std::exception& msg) {
        QTextStream(stdout) << msg.what() << '\n';
    }

}

void DepthProfileVizualizerProxy::messageReceived(Controller::BottomTransportMessage message)
{

}
