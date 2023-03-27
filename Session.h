#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include "UserBase.h"

using boost::asio::ip::tcp;
using string_pair = std::pair<std::string, std::string>;

/// <summary>
/// Class Session - establishes a network connection.
/// </summary>
/// <param name="socket">Network socket.</param>
/// <param name="sender">Shared pointer to UserBase.</param>
class Session : public UserSession, public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::shared_ptr<UserBase> base)
            : _socket(std::move(socket)), _base(std::move(base)) {}

    void start();
    void write(const Message& message) override;

private:
    void do_read_header();
    void do_read_data(std::size_t data_size);
    void do_write(const char* data, std::size_t length);

    static string_pair split_login(const std::string& name, const std::string& d);

    tcp::socket _socket;
    enum { _max_data_size = 1024 * 32 };
    char _data[_max_data_size];

    std::shared_ptr<UserBase> _base;

};

#endif //SESSION_H
