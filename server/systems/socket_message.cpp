#include "socket_message.hpp"

#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

ssize_t read_from_socket(int socket, std::string& message, std::size_t message_size) {
    std::vector<char> buffer(message_size);
    const ssize_t bytes_read = recv(socket, buffer.data(), message_size);
    if (bytes_read == -1) {
        perror("Read failed");
        return false;
    }
    if (bytes_read == 0) {
        return false;
    }
    message.assign(buffer.data(), static_cast<std::size_t>(bytes_read));
    return bytes_read;
}

ssize_t write_to_socket(int socket, const std::string& message) {
    const ssize_t bytes_written =  write(socket, message.c_str(), message.length());
    if (bytes_written == -1) {
        perror("Write failed");
        return false;
    }
    return true;
}