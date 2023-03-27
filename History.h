#ifndef HISTORY_H
#define HISTORY_H

#include <vector>

#include "Message.h"

//TODO  optimize -> SQL

using history_t = std::vector<Message>;

/// <summary>
/// Class History - contains the history of messages.
/// </summary>
class History {
public:
    void add_message(const Message& message);
    history_t get_history_from(const std::string& name);
    history_t get_history_to(const std::string& name);
    history_t get_undelivered_to(const std::string& name);
    void set_delivered(std::size_t id);

private:
    history_t _history;

};

#endif //HISTORY_H
