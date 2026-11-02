#include "socket_write.hpp"

#include <cerrno>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

bool write_to_socket(int socket, const char* message, std::size_t message_size) {
    std::size_t total_written = 0;
    while (total_written < message_size) {
        const ssize_t bytes_written =
            write(socket, message + total_written, message_size - total_written);
        if (bytes_written <= 0) {
            perror("Write failed");
            return false;
        }
        total_written += static_cast<std::size_t>(bytes_written);
    }
    return true;
}
