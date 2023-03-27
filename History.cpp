#include "History.h"

void History::add_message(const Message& message) {
    _history.push_back(message);
}

history_t History::get_history_from(const std::string& name) {
    history_t result;
    for (auto& message : _history) {
        if (message.get_from() == name) {
            result.push_back(message);
        }
    }
    return result;
}

history_t History::get_history_to(const std::string& name) {
    history_t result;
    for (auto& message : _history) {
        if (message.get_to() == name) {
            result.push_back(message);
        }
    }
    return result;
}

history_t History::get_undelivered_to(const std::string &name) {
    history_t result;
    for (auto& message : _history) {
        if (message.get_to() == name && !message.is_delivered()) {
            result.push_back(message);
        }
    }
    return result;
}

void History::set_delivered(std::size_t id) {
    for (auto& message : _history) {
        if (message.get_id() == id) {
            message.set_delivered();
            break;
        }
    }
}
