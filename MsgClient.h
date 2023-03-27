#ifndef MSGCLIENT_H
#define MSGCLIENT_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <boost/asio.hpp>

#include "Message.h"

//TODO  reconnecting when connection is lost

enum class Status {
    DISCONNECTED,
    CONNECTED,
    LOGIN,
    DIALOG
};

using file_storage_t = std::map<std::size_t, std::map<std::size_t, std::string>>;
using boost::asio::ip::tcp;
namespace fs = std::filesystem;

/// <summary>
/// Class MsgClient - asynchronous messenger client.
/// </summary>
/// <param name="context">Provides the core I/O functionality.</param>
/// <param name="endpoints">Addresses of endpoints to connect .</param>
class MsgClient {
public:
    MsgClient(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints);

    void close();
    void parse_command(const std::string& command);
    void user_typing();
    void print(const std::string& message,std::ostream& out = std::cout);
    std::mutex& get_console_mutex();
    std::string get_peer_name();

private:
    void write(const Message& message);
    void connect(const tcp::resolver::results_type& endpoints);
    void do_read_header();
    void do_read_data(std::size_t data_size);
    void do_write(const char* data, std::size_t length);

    bool send_file(const std::string& filename);
    std::string receive_file(const std::string& string);

    std::string _name;
    std::string _peer_name;
    Status _status;

    boost::asio::io_context& _context;
    tcp::socket _socket;
    tcp::resolver::results_type _endpoints;
    enum { _max_data_size = 1024 * 32 };
    char _data[_max_data_size];

    static const std::size_t _max_file_size { 1000000 };
    static const std::size_t _buffer_size { 32000 };
    file_storage_t _file_storage;

    std::mutex _console_mutex;

};


#endif //MSGCLIENT_H
