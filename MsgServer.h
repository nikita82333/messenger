#ifndef MSGSERVER_H
#define MSGSERVER_H

#include "Session.h"

/// <summary>
/// Class MsgServer - asynchronous messenger server.
/// </summary>
/// <param name="io_context">Provides the core I/O functionality.</param>
/// <param name="port">Network port.</param>
class MsgServer {
public:
    MsgServer(boost::asio::io_context& io_context, short port);

private:
    void do_accept();

    tcp::acceptor _acceptor;
    std::shared_ptr<UserBase> _base;

};

#endif //MSGSERVER_H
