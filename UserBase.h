#ifndef CLIENTBASE_H
#define CLIENTBASE_H

#include <map>
#include <memory>
#include <queue>
#include <mutex>

#include "History.h"

//TODO  SQL-base

/// <summary>
/// Class UserSession - virtual class for organizing storage and access to user sessions.
/// </summary>
class UserSession {
public:
    virtual ~UserSession() = default;
    virtual void write(const Message& message) = 0;

protected:
    std::string _name;//

};

using user_session_ptr = std::shared_ptr<UserSession>;

/// <summary>
/// Class UserBase - base of users.
/// </summary>
class UserBase {
public:

    bool is_user(const std::string& name);
    bool is_user_online(const std::string& name);

    void user_reg(const std::string& name, const std::string& password, user_session_ptr session);
    bool user_try_in(const std::string& name, const std::string& password, user_session_ptr session);
    void deliver(Message& message);
    void user_out(const std::string& name);

    void set_delivered(std::size_t id);

private:
    struct User {
        std::string name;
        std::string password;
        user_session_ptr session {nullptr};
    };

    std::size_t _id {0};

    std::map<std::string, User> _users;
    History _history;
    std::mutex _users_mutex;
    std::mutex _history_mutex;

};

#endif //CLIENTBASE_H
