#include "UserBase.h"

bool UserBase::is_user(const std::string& name) {
    std::lock_guard lock(_users_mutex);
    return _users.count(name) == 1;
}

bool UserBase::is_user_online(const std::string &name) {
    std::lock_guard lock(_users_mutex);
    if (auto it = _users.find(name); it != _users.end()) {
        if (it->second.session != nullptr) {
            return true;
        }
    }
    return false;
}

void UserBase::user_reg(const std::string& name, const std::string& password, user_session_ptr session) {
    std::lock_guard lock(_users_mutex);
    _users.emplace(name, User{name, password, std::move(session)});
}

bool UserBase::user_try_in(const std::string& name, const std::string& password, user_session_ptr session) {
    std::lock_guard lock_users(_users_mutex);
    if (auto it = _users.find(name); it != _users.end()) {
        if (it->second.password == password) {
            it->second.session = std::move(session);
            it->second.session->write(Message(0, Type::LOGIN, name, "to", "OK"));
            std::lock_guard lock_history(_history_mutex);
            for (auto& msg : _history.get_undelivered_to(name)) {
                it->second.session->write(msg);
            }
            return true;
        }
    }
    return false;
}

void UserBase::deliver(Message& message) {
    if (message.get_id() == 0 && message.get_type() != Type::USER_TYPING) {
        message.set_id(++_id);
        std::lock_guard lock(_history_mutex);
        _history.add_message(message);
    }
    std::lock_guard lock(_users_mutex);
    if (auto it = _users.find(message.get_to()); it != _users.end()) {
        if (it->second.session != nullptr) {
            it->second.session->write(message);
        }
    }
}

void UserBase::user_out(const std::string& name) {
    std::lock_guard lock(_users_mutex);
    if (auto it = _users.find(name); it != _users.end()) {
        it->second.session = nullptr;
    }
}

void UserBase::set_delivered(std::size_t id) {
    std::lock_guard lock(_history_mutex);
    _history.set_delivered(id);
}



