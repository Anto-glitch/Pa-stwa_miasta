#pragma once

#include <vector>
#include <string>
#include <mutex>

using namespace std;

struct Player {
  int socket_id;
  string nickname;
  bool ready;
  bool gameStarted;
  bool timeNotified;
  bool answersSubmitted;
  double answerTime=0;
};

enum class RoundState {
  WAITING_FOR_PLAYERS,
  GAME,
  VOTING
};

struct Gamestate {
  RoundState roundState;
  char random_letter;
  bool timerStarted;
  mutex m;
  vector<Player> players;
};

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState,
                  vector<string> categories);
