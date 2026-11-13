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
  mutex m;
  vector<Player> players;
};

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState, char random_letter,
                  vector<string> categories);
