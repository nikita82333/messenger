#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <ctime>

enum class Type {
    REG,
    LOGIN,
    DIAL,
    MSG,
    FILE,
    USER_TYPING,
    DELIVERED
};

/// <summary>
/// Class Message - stores all information about the message.
/// </summary>
class Message {
public:
    enum {header_size = 5};
    explicit Message(const std::string& message);
    Message(std::size_t id, Type type, std::string from, std::string to = "to", std::string body = "body");
    Message() = delete;

    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] std::size_t get_id() const;
    Type get_type();
    std::string get_from();
    std::string get_to();
    [[nodiscard]] std::time_t get_time() const;
    std::string get_time_str();
    std::string get_body();
    [[nodiscard]] bool is_delivered() const;
    void set_id(std::size_t id);
    void set_delivered();

private:

    static Type string_to_type(const std::string& type);
    static std::string time_to_string(std::time_t *time);

    std::string _header;
    std::size_t _id;
    Type _type;
    std::string _body;
    std::string _from;
    std::string _to;
    std::time_t _time;
    bool _delivered;

};

#endif //MESSAGE_H
