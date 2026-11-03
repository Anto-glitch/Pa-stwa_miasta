#include "server_connection.hpp"
#include "socket_read.hpp"


#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

int main(void) {

  char random_letter = 'k';
  // wpisz wszystkie kategorie
  vector<string> categories = {"IMIE", "NAZWISKO", "MIASTO"};

  int server_socket = create_server_socket(3333);

  vector<vector<string>> messages;
  mutex message_mutex;

  while (true) {
    int clientId = accept_client(server_socket);

    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              random_letter, ref(categories));
    t1.detach();
  }

  close(server_socket);

  return 0;
}