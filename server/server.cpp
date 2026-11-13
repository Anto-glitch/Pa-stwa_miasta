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
#include <condition_variable>
#include <algorithm>

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

  Gamestate gameState;
  gameState.gameStarted = false;
  gameState.players = vector<Player>();

  vector<vector<string>> messages;
  mutex message_mutex;

  vector<int> clientIds;

  while (true) {
    int clientId = accept_client(serverId);
    if (clientId == -1) {
      continue;
    }
    clientIds.push_back(clientId);
    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              ref(gameState), random_letter, categories);
    t1.detach();

    lock_guard<mutex> lock(gameState.m);
    int readyPlayers = count_if(gameState.players.begin(),gameState.players.end(),[](Player &p){return p.ready;});
    if(gameState.players.size()>=2 && readyPlayers == gameState.players.size()){ 
      gameState.gameStarted = true;
    }
  }
  close_socket(serverId);

  return 0;
}
