#include <iostream>
#include <thread>
#include <string>
#include <queue>

#ifdef WIN32
#include <conio.h>
#else
#include "conio.h/conio.h"
#endif

#include "MsgClient.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        if(argc != 3) {
            std::cerr << "Usage: client <server> <port>" << std::endl;
            std::cerr << " - <server> - ip address of message server." << std::endl;
            std::cerr << " - <port> - tcp port number for connection to server." << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(argv[1], argv[2]);
        MsgClient client(io_context, endpoints);
        std::thread thread([&io_context](){ io_context.run(); });

        client.get_console_mutex().lock();
        std::cout << "List of commands:" << std::endl;
        std::cout << "reg <name> <password> - registration on the message server," << std::endl;
        std::cout << "login <name> <password> - login on the message server," << std::endl;
        std::cout << "dail <name> - start a dialog with the user <name>," << std::endl;
        std::cout << "[msg] <message> - send <message> to a previously started dialog." << std::endl;
        std::cout << "file <file_name> - send file <file_name> to a previously started dialog." << std::endl;
        std::cout << "exit - exit." << std::endl;
        client.get_console_mutex().unlock();
        std::string command;
        std::getline(std::cin, command);

        while (command != "exit") {
            client.parse_command(command);
            command.clear();

            //std::getline(std::cin, command);
            int ch, count = 0;
            while ((ch = getch()) != 13) {
                if (count == 0) {
                    client.get_console_mutex().lock();
                    std::cout << "to " << client.get_peer_name() << "> ";
                }
                if (count % 10 == 9) {
                    client.user_typing();
                }
                if (std::isprint(ch)) {
                    std::cout << static_cast<char>(ch);
                    ++count;
                    command += static_cast<char>(ch);
                } else if (ch == 8) {
                    std::cout << "\b \b";
                    command.resize(command.length() - 1);
                }
            }
            std::cout << std::endl;
            client.get_console_mutex().unlock();
        }

        client.close();
        thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
