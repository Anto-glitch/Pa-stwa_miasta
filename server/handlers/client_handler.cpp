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
                  mutex &message_mutex, Gamestate &gameState, char random_letter,
                  vector<string> categories) {
  write_to_socket(clientId, string(1, random_letter));

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
  vector<string> answers =
      collectAnswers(clientId, accumulated_text, random_letter, categories);
  saveAnswers(move(answers), nickname, messages, message_mutex);
  removePlayer(clientId, gameState);
  close_socket(clientId);
}
