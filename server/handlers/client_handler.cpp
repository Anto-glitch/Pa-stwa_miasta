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

bool readLineFromSocket(int clientId, string &accumulated_text,
                        string &out_line) {
  char buffer[1024];
  while (true) {
    size_t pos = accumulated_text.find('\n');
    if (pos != string::npos) {
      out_line = accumulated_text.substr(0, pos);
      if (!out_line.empty() && out_line.back() == '\r') {
        out_line.pop_back();
      }
      accumulated_text.erase(0, pos + 1);
      return true;
    }

    ssize_t bytes_read = read_from_socket(clientId, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      return false;
    }
    accumulated_text.append(buffer, bytes_read);
  }
}

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, Gamestate &gameState, char random_letter,
                  vector<string> categories) {
  //Wyslanie wylosowanej litery do klienta zaraz po polaczeniu
  write_to_socket(clientId, string(1, random_letter));

  // Odbior nicku od klienta
  string accumulated_text = "";
  string nickname = "";

  // Klient sie rozlaczyl
  if (!readLineFromSocket(clientId, accumulated_text, nickname)) {
    close_socket(clientId);
    return;
  }

  if (nickname.empty()) {
    nickname = "Gracz_" + to_string(clientId);
  }

  cout << "Nowy gracz dolaczyl: " << nickname << " (ID: " << clientId << ")"
       << endl;

       //waiting for all players to be ready
  string ready = "";
   if(readLineFromSocket(clientId, accumulated_text, ready)) {
    if (ready == "READY") {
      {
        unique_lock<mutex> lock(gameState.m);
        gameState.players.push_back({clientId, nickname, true}); 
      }
    }
  }

  pollfd pfds[1];
  pfds[0].fd = clientId;
  pfds[0].events = POLLIN;

  while(!gameState.gameStarted) {
    
    {
      lock_guard<mutex> lock(gameState.m);
      if(gameState.gameStarted) {
        break;
      }
    }
    
    int ret = poll(pfds, 1, 100); // timeout

    if(pfds[0].revents & POLLIN) {
      if (ready == "EXIT" || !readLineFromSocket(clientId, accumulated_text, ready)) { 
        for (auto &player : gameState.players) {
          if (player.socket_id == clientId) {
            unique_lock<mutex> lock(gameState.m);
            gameState.players.erase(std::remove(gameState.players.begin(), gameState.players.end(), player), gameState.players.end());
            close_socket(clientId);
            return;
         }
        }
      }
    }
    if( pfds[0].revents & (POLLHUP | POLLERR))
    {
      for (auto &player : gameState.players) {
          if (player.socket_id == clientId) {
            unique_lock<mutex> lock(gameState.m);
            gameState.players.erase(std::remove(gameState.players.begin(), gameState.players.end(), player), gameState.players.end());
            close_socket(clientId);
            return;
         }
        }
    }

  }

  vector<string> answers;
  int turns = 0;

  while (turns < categories.size()) {
    string prompt =
        "Podaj " + categories[turns] + " (kazde zatwierdz enterem):\n";
    write_to_socket(clientId, prompt);

    string line;
    if (!readLineFromSocket(clientId, accumulated_text, line)) {
      break; // Klient sie rozlaczyl
    }

    if (line == "exit") {
      break;
    }

    line = capitalize(line);

    // ZAMIAST break: jesli odpowiedz jest bledna, prosimy o nia ponownie (bez
    // zwiekszania turns)
    if (line.empty() || line[0] != random_letter) {
      continue;
    }

    answers.push_back(line);
    turns++;
  }

  // Poinformuj klienta, ze to koniec kategorii
  write_to_socket(clientId, "EXIT\n");

  if (!answers.empty()) {
    lock_guard<mutex> lock(message_mutex);
    answers.insert(answers.begin(), nickname); // answers[0] to nick gracza
    messages.push_back(answers);

    cout << "Ilosc zatwierdzonych pakietow odpowiedzi: " << messages.size()
         << endl;
    cout << "Zestawienie odpowiedzi wszystkich graczy: " << endl;

    display_answers(messages);
  }

  close_socket(clientId);
}
