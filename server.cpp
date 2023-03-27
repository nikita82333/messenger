#include <iostream>

#include "MsgServer.h"

int main(int argc, char* argv[]) {

    try {
        if (argc != 2) {
            std::cerr << "Usage: messenger_server <port>" << std::endl;
            std::cerr << " - <port> - tcp port number for incoming connections." << std::endl;

            return 1;
        }

        boost::asio::io_context io_context;
        MsgServer server(io_context, std::atoi(argv[1]));
        io_context.run();

    } catch (const std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }

    return 0;

}
