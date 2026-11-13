#include "client_handler.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <thread>
#include <poll.h>

#include "../systems/server_connection.hpp"
#include "../systems/socket_message.hpp"
#include "../utils/utils.hpp"

using namespace std;

enum class ReadStatus {
  SUCCESS,
  ERROR,
  PARTIAL
};

ReadStatus readLineFromSocket(int clientId, string &accumulated_text,
                              string &out_line, int flags = 0) {
  char buffer[1024];
  while (true) {
    size_t pos = accumulated_text.find('\n');
    if (pos != string::npos) {
      out_line = accumulated_text.substr(0, pos);
      if (!out_line.empty() && out_line.back() == '\r') {
        out_line.pop_back();
      }
      accumulated_text.erase(0, pos + 1);
      return ReadStatus::SUCCESS;
    }

    ssize_t bytes_read =
        read_from_socket(clientId, buffer, sizeof(buffer) - 1, flags);
    if (bytes_read <= 0) {
      return flags == 1 ? ReadStatus::PARTIAL : ReadStatus::ERROR;
    }
    accumulated_text.append(buffer, bytes_read);
    if (flags == 1 && accumulated_text.find('\n') == string::npos) {
      return ReadStatus::PARTIAL;
    }
  }
}

bool registerPlayer(int clientId, string &nickname, Gamestate &gameState) {
  if (nickname.empty()) {
    nickname = "Gracz_" + to_string(clientId);
  }

  lock_guard<mutex> lock(gameState.m);
  const auto existingPlayer = find_if(
      gameState.players.begin(), gameState.players.end(),
      [&nickname](const Player &player) {
        return player.nickname == nickname;
      });

  if (existingPlayer != gameState.players.end()) {
    write_to_socket(clientId, "NICK_TAKI_SAM\n");
    return false;
  }

  gameState.players.push_back({clientId, nickname, false});
  return true;
}

bool markPlayerReady(int clientId, Gamestate &gameState) {
  lock_guard<mutex> lock(gameState.m);
  const auto player = find_if(
      gameState.players.begin(), gameState.players.end(),
      [clientId](const Player &currentPlayer) {
        return currentPlayer.socket_id == clientId;
      });

  if (player == gameState.players.end()) {
    return false;
  }

  player->ready = true;
  return true;
}

void removePlayer(int clientId, Gamestate &gameState) {
  lock_guard<mutex> lock(gameState.m);
  gameState.players.erase(
      remove_if(gameState.players.begin(), gameState.players.end(),
                [clientId](const Player &player) {
                  return player.socket_id == clientId;
                }),
      gameState.players.end());
}

bool isGameStarted(Gamestate &gameState) {
  lock_guard<mutex> lock(gameState.m);
  return gameState.gameStarted;
}

bool waitForGameStart(int clientId, string &accumulated_text,
                      Gamestate &gameState) {
  string ready;
  if (readLineFromSocket(clientId, accumulated_text, ready) !=
      ReadStatus::SUCCESS) {
    return false;
  }

  if (ready != "READY" || !markPlayerReady(clientId, gameState)) {
    return false;
  }

  pollfd poll_descriptor{clientId, POLLIN, 0};
  while (!isGameStarted(gameState)) {
    poll_descriptor.revents = 0;
    if(poll(&poll_descriptor, 1, 100) < 0) {
      perror("Poll failed");
      return false;
    }

    if (poll_descriptor.revents & (POLLHUP | POLLERR)) {
      return false;
    }
    if (poll_descriptor.revents & POLLIN) {
      const ReadStatus status =
          readLineFromSocket(clientId, accumulated_text, ready, 1);
      if (status == ReadStatus::ERROR ||
          (status == ReadStatus::SUCCESS && ready == "EXIT")) {
        return false;
      }
    }
  }

  return true;
}

vector<string> collectAnswers(int clientId, string &accumulated_text,
                               char random_letter,
                               const vector<string> &categories) {
  vector<string> answers;
  size_t categoryIndex = 0;

  while (categoryIndex < categories.size()) {
    const string prompt =
        "Podaj " + categories[categoryIndex] + " (kazde zatwierdz enterem):\n";
    write_to_socket(clientId, prompt);

    string line;
    if (readLineFromSocket(clientId, accumulated_text, line) !=
            ReadStatus::SUCCESS ||
        line == "EXIT") {
      break;
    }

    line = capitalize(line);
    if (line.empty() || line[0] != random_letter) {
      continue;
    }

    answers.push_back(line);
    ++categoryIndex;
  }

  return answers;
}

void saveAnswers(vector<string> answers, const string &nickname,
                 vector<vector<string>> &messages, mutex &message_mutex) {
  if (answers.empty()) {
    return;
  }

  lock_guard<mutex> lock(message_mutex);
  answers.insert(answers.begin(), nickname);
  messages.push_back(answers);

  cout << "Ilosc zatwierdzonych pakietow odpowiedzi: " << messages.size()
       << endl;
  cout << "Zestawienie odpowiedzi wszystkich graczy: " << endl;
  display_answers(messages);
}

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState, char random_letter,
                  vector<string> categories) {


                    
  write_to_socket(clientId, string(1, random_letter));

  string accumulated_text;
  string nickname;

  while(true) {
    if (readLineFromSocket(clientId, accumulated_text, nickname) !=
        ReadStatus::SUCCESS) {
      close_socket(clientId);
      return;
    } 

    if (registerPlayer(clientId, nickname, gameState)) {
      break;
    }
    write_to_socket(clientId, "BAD_USERNAME:\n");
  }

  if (!waitForGameStart(clientId, accumulated_text, gameState)) {
    removePlayer(clientId, gameState);
    close_socket(clientId);
    return;
  }

  vector<string> answers =
      collectAnswers(clientId, accumulated_text, random_letter, categories);
  saveAnswers(move(answers), nickname, messages, message_mutex);
  close_socket(clientId);
}
