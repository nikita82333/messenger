#include "MsgClient.h"

MsgClient::MsgClient(boost::asio::io_context &io_context, const tcp::resolver::results_type& endpoints)
        : _context(io_context), _socket(io_context), _endpoints(endpoints) {
    connect(endpoints);
}

void MsgClient::write(const Message& message) {
    boost::asio::post(_context, [this, message]() {
        std::string msg{message.serialize()};
        do_write(msg.c_str(), msg.length());
    });
}

void MsgClient::close() {
    boost::asio::post(_context, [this]() { _socket.close(); });
}

void MsgClient::connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, const tcp::endpoint&) {
        if (!ec) {
            _status = Status::CONNECTED;
            print("Connection successful!");
            do_read_header();
        } else {
            print("Error: server offline!", std::cerr);
            _socket.close();
            _status = Status::DISCONNECTED;
        }
    });
}

void MsgClient::do_read_header() {
    boost::asio::async_read(_socket, boost::asio::buffer(_data, Message::header_size),
        [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                std::size_t data_size = std::stoi(_data);
                if (data_size <= _max_data_size) {
                    do_read_data(data_size);
                }
            } else {
                print("Error: connection lost!", std::cerr);
                _socket.close();
                _status = Status::DISCONNECTED;
            }
        });
}

void MsgClient::do_read_data(std::size_t data_size) {
    boost::asio::async_read(_socket, boost::asio::buffer(_data, data_size),
                            [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                Message message{std::string(_data, length)};
                if (message.get_type() == Type::REG) {
                    if (message.get_body() == "OK") {
                        print("Register successful.");
                        _name = message.get_from();
                        _status = Status::LOGIN;
                    } else {
                        print(message.get_body(), std::cerr);
                    }
                } else if (message.get_type() == Type::LOGIN) {
                    if (message.get_body() == "OK") {
                        print("Login successful.");
                        _name = message.get_from();
                        _status = Status::LOGIN;
                    } else {
                        print(message.get_body(), std::cerr);
                    }
                } else if (message.get_type() == Type::DIAL) {
                    if (message.get_body() == "OK") {
                        print("Dialog started.");
                        _peer_name = message.get_to();
                        _status = Status::DIALOG;
                    } else {
                        print(message.get_body(), std::cerr);
                    }
                } else if (_status == Status::DIALOG || _status == Status::LOGIN) {
                    if (message.get_type() == Type::MSG) {
                        write(Message(message.get_id(), Type::DELIVERED, _name));
                        print(message.get_time_str() + " " + message.get_from() + ": " + message.get_body());
                    } else if (message.get_type() == Type::USER_TYPING) {
                        print(message.get_from() + ": " + message.get_body());
                    } else if (message.get_type() == Type::FILE) {
                        write(Message(message.get_id(), Type::DELIVERED, _name));
                        std::string filename{receive_file(message.get_body())};
                        if (!filename.empty()) {
                            print(message.get_time_str() + " " + message.get_from() + ": "
                                + "Sent file \"" + filename + "\"");
                        }
                    }
                }
                do_read_header();
            }
            else {
                print("Error: connection lost!", std::cerr);
                _socket.close();
                _status = Status::DISCONNECTED;
            }
    });
}

void MsgClient::do_write(const char* data, std::size_t length) {
    boost::asio::async_write(_socket, boost::asio::buffer(data, length),
        [this](boost::system::error_code ec, std::size_t/* length*/) {
        if (!ec) {

        } else {
            print("Error: connection lost!", std::cerr);
            _socket.close();
            _status = Status::DISCONNECTED;
        }
    });
}

void MsgClient::parse_command(const std::string& command) {
    std::stringstream ss(command);
    std::string part, password;
    ss >> part;
    if (part == "reg") {
        ss >> part >> password;
        if (!password.empty()) {
            write(Message(0, Type::REG, part + "@" + password));
        } else {
            print("Warning. Password cannot be empty!", std::cerr);
        }
    } else if (part == "login") {
        ss >> part >> password;
        if (!password.empty()) {
            write(Message(0, Type::LOGIN, part + "@" + password));
        } else {
            print("Warning. Password cannot be empty!", std::cerr);
        }
    } else if (part == "dial") {
        if (_status == Status::LOGIN || _status == Status::DIALOG) {
            ss >> part;
            write(Message(0, Type::DIAL, _name, part));
        } else {
            print("Warning. You must register and login first!", std::cerr);
        }
    } else if (_status == Status::DIALOG) {
        if (part == "file") {
            ss >> part;
            if (!send_file(part)) {
                print("Warning. File does not exist or its size exceeds "
                      + std::to_string(_max_file_size / 1000000.f) + " mb!", std::cerr);
            }
        } else {
            if (part == "msg") {
                std::stringstream buf;
                buf << ss.rdbuf();
                part = buf.str().substr(1);
            } else {
                part = ss.str();
            }
            if (!part.empty()) {
                write(Message(0, Type::MSG, _name, _peer_name, part));
            }
        }
    } else {
        print("Warning. You must register, login and choose a recipient first!");
    }
}

void MsgClient::user_typing() {
    if (_status == Status::DIALOG) {
        write(Message(0, Type::USER_TYPING, _name, _peer_name, "Typing a message..."));
    }
}

bool MsgClient::send_file(const std::string& filename) {
    fs::path path{"./" + _name + "/" + filename};
    if (!fs::exists(path)) {
        path = "./" + filename;
        if (!fs::exists(path)) {
            return false;
        }
    }

    std::size_t filesize{fs::file_size(path)};
    if (filesize > _max_file_size) {
        return false;
    }
    std::ifstream file(path, std::ios::binary);

    if (file.is_open()) {
        std::string buffer(_buffer_size, '\0');
        std::size_t i_block = 0;
        std::string file_id;
        std::size_t blocks_count = std::ceil(static_cast<long double>(filesize) / _buffer_size);
        while (file) {
            file.read(buffer.data(), _buffer_size);
            if (i_block == 0) {
                file_id = std::to_string(std::hash<std::string>{}(std::string{buffer}));
            }
            if (file.gcount() != 0) {
                std::string block{file_id};
                block.append(" " + filename + " " + std::to_string(i_block) + " " + std::to_string(blocks_count) + " ");
                block.append(buffer, 0, file.gcount());
                write(Message(0, Type::FILE, _name, _peer_name, block));
            }
            ++i_block;
        }
        file.close();
        return true;
    }
    return false;
}

std::string MsgClient::receive_file(const std::string& string) {
    std::stringstream ss{string};
    std::string filename;
    std::size_t file_id, i_block, blocks_count;
    ss >> file_id >> filename >> i_block >> blocks_count;
    std::stringstream buf;
    buf << ss.rdbuf();

    if (auto it = _file_storage.find(file_id); it != _file_storage.end()) {
        it->second.emplace(i_block, buf.str().substr(1));
    } else {
        _file_storage[file_id].emplace(i_block, buf.str().substr(1));
    }

    if (_file_storage[file_id].size() == blocks_count) {
        fs::create_directory(fs::path{"./" + _name + "/"});
        std::ofstream file(fs::path{"./" + _name + "/" + filename}, std::ios::binary);
        if (file.is_open()) {
            for (const auto& [id, block] : _file_storage[file_id]) {
                file.write(block.data(), block.length());
            }
            file.close();
            _file_storage.erase(file_id);
            return filename;
        }
    }
    return {};
}

std::mutex& MsgClient::get_console_mutex() {
    return _console_mutex;
}

void MsgClient::print(const std::string &message, std::ostream& out) {
    std::lock_guard lock(_console_mutex);
    out << message << std::endl;
}

std::string MsgClient::get_peer_name() {
    return _peer_name;
}


