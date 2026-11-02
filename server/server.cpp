#include "server_connection.hpp"
#include "socket_read.hpp"

#include <iostream>
#include <string>
#include <vector>

int main() {
    const int server_id = create_server_socket(3333);
    if (server_id == -1) {
        return 1;
    }

    const int client_id = accept_client(server_id);
    if (client_id == -1) {
        close_socket(server_id);
        return 1;
    }

    constexpr std::size_t message_size = 10;
    const std::string close_message = "close_conn";
    std::vector<std::string> messages;
    std::string received_message;

    while (read_from_socket(client_id, received_message, message_size)) {
        if (received_message == close_message) {
            break;
        }
        messages.push_back(received_message);
    }

    std::cout << messages.size() << '\n';
    for (const std::string& message : messages) {
        std::cout << message << '\n';
    }

    close_socket(client_id);
    close_socket(server_id);
    return 0;
}
