

#include <cstddef>
#include <string>

ssize_t read_from_socket(int socket, std::string& message, std::size_t message_size);
ssize_t write_to_socket(int socket, const std::string& message); 