#pragma once

int create_server_socket(int port_number);
int accept_client(int server_socket);
void close_socket(int socket);
