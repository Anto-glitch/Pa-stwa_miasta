#pragma once

#include <cstddef>

bool write_to_socket(int socket, const char* message, std::size_t message_size);
