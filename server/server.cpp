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

using namespace std;

struct Player 
{
  int socket_id;
  string nickname;
  bool ready;
};

struct Gamestate{
  condition_variable cv;
  bool gameStarted;
  mutex m;
  vector<Player> players;
};

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
    thread t1(handleClient, clientId, ref(messages), ref(message_mutex), ref(players), ref(players_mutex), ref(gameState), random_letter,
          ref(gameState),random_letter, ref(categories));
    t1.detach();

    lock_guard<mutex> lock(gameState.m);
    if(gameState.players.size() >= 2) {
      gameState.gameStarted = true;
      gameState.cv.notify_all();
    }
  }
  close_socket(serverId);

  return 0;
}
