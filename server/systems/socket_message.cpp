#include "socket_message.hpp"

#include <cstdio>
#include <sys/socket.h>
#include <vector>

ssize_t read_from_socket(int socket, std::string& message, std::size_t message_size) {
    const ssize_t bytes_read = recv(socket, message.c_str(), message_size, MSG_WAITALL);
    if (bytes_read == -1) {
        perror("Read failed");
        return -1;
    }
    message.assign(message.c_str(), static_cast<std::size_t>(bytes_read));
    return bytes_read;
}

ssize_t write_to_socket(int socket, const std::string& message) {
    const ssize_t bytes_written =  write(socket, message.c_str(), message.length());
    if (bytes_written == -1) {
        perror("Write failed");
        return -1;
    }
    return bytes_written;
}