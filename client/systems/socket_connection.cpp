#include "socket_connection.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int create_socket() {
    const int socket_id = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        perror("Socket creation failed");
    }
    return socket_id;
}

bool create_connection(int socket_id, int port_number, const char* server_address) {
    sockaddr_in client_addr{};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port_number);

    if (inet_pton(AF_INET, server_address, &client_addr.sin_addr) != 1) {
        perror("Invalid server address");
        return false;
    }

    if (connect(socket_id, reinterpret_cast<sockaddr*>(&client_addr), sizeof(client_addr)) == -1) {
        perror("Failed to connect");
        return false;
    }
    return true;
}

void disconnect_connection(int& socket) {
    if (socket == -1) {
        return;
    }

    shutdown(socket, SHUT_RDWR);
    close(socket);
    socket = -1;
}
