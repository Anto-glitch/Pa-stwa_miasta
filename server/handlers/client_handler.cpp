#include "client_handler.hpp"

#include <thread>

#include "answers/answer_handler.hpp"
#include "io/read_line.hpp"
#include "players/player_registry.hpp"
#include "waiting_room/waiting_room.hpp"
#include "../systems/server_connection.hpp"
#include "../systems/socket_message.hpp"

using namespace std;

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState,
                  vector<string> categories) {
  write_to_socket(clientId, "CHECK\n");

  string accumulated_text;
  string nickname;

  while (true) {
    if (readLineFromSocket(clientId, accumulated_text, nickname) !=
        ReadStatus::SUCCESS) {
      close_socket(clientId);
      return;
    }

    if (registerPlayer(clientId, nickname, gameState)) {
      break;
    }
  }

  if (!waitForGameStart(clientId, accumulated_text, gameState)) {
    removePlayer(clientId, gameState);
    close_socket(clientId);
    return;
  }
  write_to_socket(clientId, "START\n");
  write_to_socket(clientId,
                  "LETTER " + string(1, gameState.random_letter) + "\n");

  bool sendTime = false;
  {
    lock_guard<mutex> lock(gameState.m);
    for (Player &player : gameState.players) {
      if (player.socket_id == clientId) {
        player.gameStarted = true;
        if (gameState.timerStarted && !player.timeNotified) {
          player.timeNotified = true;
          sendTime = true;
        }
        break;
      }
    }
  }
  if (sendTime) {
    write_to_socket(clientId, "TIME\n");
  }

  vector<string> answers =
      collectAnswers(clientId, accumulated_text, gameState.random_letter,
                     categories, gameState);
  saveAnswers(move(answers), nickname, messages, message_mutex);

  {
    lock_guard<mutex> lock(gameState.m);
    for (Player &player : gameState.players) {
      if (player.socket_id == clientId) {
        player.answersSubmitted = true;
        break;
      }
    }
  }
}
