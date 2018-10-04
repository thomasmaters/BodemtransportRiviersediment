namespace Communication::TCP
{
template <typename Type>
void TCPSession::handleResponse(Type response_indentifier,
                                bool has_response_head_and_body,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferd)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> SEND( " << bytes_transferd << ")" << std::endl;
#endif
    if (!error)
    {
        boost::asio::streambuf aBuffer;
        if
            constexpr(std::is_same<Type, char>::value)
            {
                boost::asio::async_read_until(getSocket(),
                                              aBuffer,
                                              response_indentifier,
                                              boost::bind(&Communication::TCP::TCPSession::handleReceivedResponse,
                                                          shared_from_this(),
                                                          has_response_head_and_body,
                                                          boost::asio::placeholders::error,
                                                          boost::asio::placeholders::bytes_transferred));
            }
        else
        {
            boost::asio::async_read(getSocket(),
                                    boost::asio::buffer(data_, response_indentifier),
                                    boost::bind(&TCPSession::handleReceivedResponse,
                                                shared_from_this(),
                                                has_response_head_and_body,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
#ifdef ENABLE_IO_DEBUG
        std::cout << "TCPSession -> Failed to get response: " << error.message() << std::endl;
#endif
    }
}

template <typename Type>
void TCPSession::handleOutgoingConnection(const boost::asio::mutable_buffer& message,
                                          Type response_indentifier,
                                          bool has_response_head_and_body)
{
#ifdef ENABLE_IO_DEBUG
    std::cout << "TCPSession -> SENDING( " << message.size() << ")" << std::endl;
#endif
    boost::asio::async_write(getSocket(),
                             boost::asio::buffer(message),
                             boost::bind(&TCPSession::handleResponse<Type>,
                                         shared_from_this(),
                                         response_indentifier,
                                         has_response_head_and_body,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}
}
