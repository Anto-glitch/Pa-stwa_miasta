#include "socket_read.hpp"

#include <cstdio>
#include <sys/socket.h>
#include <vector>

bool read_from_socket(int socket, std::string& message, std::size_t message_size) {
    std::vector<char> buffer(message_size);
    const ssize_t bytes_read = recv(socket, buffer.data(), message_size, MSG_WAITALL);
    if (bytes_read == -1) {
        perror("Read failed");
        return false;
    }
    if (bytes_read == 0) {
        return false;
    }

    message.assign(buffer.data(), static_cast<std::size_t>(bytes_read));
    return true;
}
