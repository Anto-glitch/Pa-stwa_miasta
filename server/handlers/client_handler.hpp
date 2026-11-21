#pragma once

#include <vector>
#include <string>
#include <mutex>

using namespace std;

struct Player {
  int socket_id;
  string nickname;
  bool ready;
};

struct Gamestate {
  bool gameStarted;
  char random_letter;
  mutex m;
  vector<Player> players;
};

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState,
                  vector<string> categories);
