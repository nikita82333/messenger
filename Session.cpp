#include "Session.h"

void Session::start() {
    do_read_header();
}

void Session::write(const Message& message) {
    std::string msg{message.serialize()};
    do_write(msg.c_str(), msg.length());
}

void Session::do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_data, Message::header_size),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                std::size_t data_size = std::stoi(_data);
                if (data_size <= _max_data_size) {
                    do_read_data(data_size);
                }
            } else {
                std::cout << "User out: " << _name << std::endl;
                _base->user_out(_name);
            }
        });
}

void Session::do_read_data(std::size_t data_size) {
    auto self(shared_from_this());
    boost::asio::async_read(_socket, boost::asio::buffer(_data, data_size),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                Message message{std::string(_data, length)};
                if (message.get_type() == Type::REG) {
                    auto [name, password] = split_login(message.get_from(), "@");
                    if (_base->is_user(name)) {
                        write(Message(0, message.get_type(), name, message.get_to(),
                                      "Error. This name is already used."));
                    } else {
                        _base->user_out(_name);
                        _name = name;
                        _base->user_reg(_name, password, shared_from_this());
                        write(Message(0, message.get_type(), name, message.get_to(), "OK"));
                    }
                } else if (message.get_type() == Type::LOGIN) {
                    auto [name, password] = split_login(message.get_from(), "@");
                    if (!_base->is_user(name)) {
                        write(Message(0, message.get_type(), name, message.get_to(),
                                      "Error. User with this name is not registered."));
                    } else {
                        if (_base->user_try_in(name, password, shared_from_this())) {
                            _base->user_out(_name);
                            _name = name;
                        } else {
                            write(Message(0, message.get_type(), name, message.get_to(),
                                          "Error. Password is incorrect."));
                        }
                    }
                } else if (message.get_type() == Type::DIAL) {
                    if (!_base->is_user(message.get_to())) {
                        write(Message(0, Type::DIAL, message.get_from(), message.get_to(),
                                      "Error. User with this name is not registered."));
                    } else {
                        write(Message(0, Type::DIAL, message.get_from(), message.get_to(), "OK"));
                    }
                }

                if (message.get_type() == Type::MSG || message.get_type() == Type::FILE
                    || message.get_type() == Type::USER_TYPING) {
                    _base->deliver(message);
                }
                if (message.get_type() == Type::DELIVERED) {
                    _base->set_delivered(message.get_id());
                }

                do_read_header();

            } else {
                std::cout << "User out: " << _name << std::endl;
                _base->user_out(_name);
            }
        });
}

void Session::do_write(const char* data, std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(data, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {

            } else {
                std::cout << "User out: " << _name << std::endl;
                _base->user_out(_name);
            }
        });
}

string_pair Session::split_login(const std::string& name, const std::string& d) {
    auto pos = name.find(d);
    return {name.substr(0, pos), name.substr(pos + 1)};
}
