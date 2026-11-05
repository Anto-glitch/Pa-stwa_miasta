#include "utils/utils.hpp"
#include "systems/server_connection.hpp"
#include "systems/socket_message.hpp"
#include "handlers/client_handler.hpp"


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

  char random_letter = generateRandomChar();
  cout << "Wylosowana litera: " << random_letter << endl;

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
