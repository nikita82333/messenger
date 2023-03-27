#include "MsgServer.h"

MsgServer::MsgServer(boost::asio::io_context& io_context, short port)
        : _acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    _base = std::make_shared<UserBase>();
    do_accept();
}

void MsgServer::do_accept() {
    _acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), _base)->start();
            }

            do_accept();
        });
}
