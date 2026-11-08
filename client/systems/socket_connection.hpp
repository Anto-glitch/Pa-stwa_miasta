#pragma once

int create_socket();
bool create_connection(int socket, int port_number, const char* server_address);
void disconnect_connection(int& socket);
