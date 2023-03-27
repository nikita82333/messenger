#include <sstream>
#include <iomanip>
#include <utility>
#include <chrono>

#include "Message.h"

Message::Message(const std::string& message) {
    std::stringstream ss(message);
    std::string type, time, word;
    ss >> _id >> type >> _from >> _to >> time;
    _type = string_to_type(type);
    _time = std::stoll(time);
    std::stringstream buf;
    buf << ss.rdbuf();
    _body = buf.str().substr(1);
    _delivered = false;
}

Message::Message(std::size_t id, Type type, std::string from, std::string to, std::string body)
    : _id(id), _type(type), _body(std::move(body)), _from(std::move(from)), _to(std::move(to)) {
    _time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    _delivered = false;
}

std::string Message::serialize() const {
    std::string type;
    switch (_type) {
        case Type::REG:
            type = "REG";
            break;
        case Type::LOGIN:
            type = "LOGIN";
            break;
        case Type::DIAL:
            type = "DIAL";
            break;
        case Type::MSG:
            type = "MSG";
            break;
        case Type::FILE:
            type = "FILE";
            break;
        case Type::USER_TYPING:
            type = "USER_TYPING";
            break;
        case Type::DELIVERED:
            type = "DELIVERED";
            break;
    }
    std::string data
        {std::to_string(_id) + " " + type + " " + _from + " " + _to + " " + std::to_string(_time) + " " + _body};

    std::stringstream header;
    header << std::setw(header_size) << std::setfill('0') << data.length();
    return header.str() + data;
}


std::string Message::time_to_string(std::time_t *time) {
    std::stringstream ss;
    std::tm* tm = localtime(time);
    ss << "[" << tm->tm_year + 1900 << "-" << std::setw(2) << std::setfill('0') << tm->tm_mon + 1 << "-"
       << std::setw(2) << std::setfill('0') << tm->tm_mday << " "
       << std::setw(2) << std::setfill('0') << tm->tm_hour << ":"
       << std::setw(2) << std::setfill('0') << tm->tm_min << ":"
       << std::setw(2) << std::setfill('0') << tm->tm_sec << "]";
    return ss.str();
}

Type Message::string_to_type(const std::string& type) {
    if (type == "REG") {
        return Type::REG;
    } else if (type == "LOGIN") {
        return Type::LOGIN;
    } else if (type == "DIAL") {
        return Type::DIAL;
    } else if (type == "MSG") {
        return Type::MSG;
    } else if (type == "FILE") {
        return Type::FILE;
    } else if (type == "USER_TYPING") {
        return Type::USER_TYPING;
    } else if (type == "DELIVERED") {
        return Type::DELIVERED;
    }
    return Type::MSG;
}

std::size_t Message::get_id() const {
    return _id;
}

Type Message::get_type() {
    return _type;
}

std::string Message::get_from() {
    return _from;
}

std::string Message::get_to() {
    return _to;
}

std::time_t Message::get_time() const {
    return _time;
}

std::string Message::get_time_str() {
    return time_to_string(&_time);
}

std::string Message::get_body() {
    return _body;
}

bool Message::is_delivered() const {
    return _delivered;
}

void Message::set_id(std::size_t id) {
    _id = id;
}

void Message::set_delivered() {
    _delivered = true;
}

