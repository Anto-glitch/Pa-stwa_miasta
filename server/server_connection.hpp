#pragma once

#include <mutex>
#include <string>
#include <vector>

int create_server_socket(int port_number);
int accept_client(int server_socket);
void close_socket(int socket);

void handleClient(int clientId, std::vector<std::vector<std::string>> &messages,
                  std::mutex &message_mutex, char random_letter,
                  std::vector<std::string> categories);
