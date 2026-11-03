#include "server_connection.hpp"

#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int create_server_socket(int port_number) {
    const int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    int option = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("Setting socket options failed");
        close(server_socket);
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }
    return server_socket;
}

int accept_client(int server_socket) {
    const int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
        perror("Accept failed");
    }
    return client_socket;
}

void close_socket(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}
