#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>


#include "systems/server_connection.hpp"
#include "utils/utills.hpp"
#include "systems/socket_message.hpp"
#include "handlers/client_handler.hpp"

using namespace std;

int main(void) {

  char random_letter = 'k';
  // wpisz wszystkie kategorie
  vector<string> categories = {"IMIE", "NAZWISKO", "MIASTO", "ROSLINA"};

  int serverId = create_server_socket(3333);
  if (serverId == -1) {
    return 1;
  }

  vector<vector<string>> messages;
  mutex message_mutex;

  while (true) {
    int clientId = accept_client(serverId);
    if (clientId == -1) {
      continue;
    }

    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              random_letter, ref(categories));
    t1.detach();
  }
  close_socket(serverId);

  return 0;
}