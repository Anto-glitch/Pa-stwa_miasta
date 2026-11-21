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
#include <chrono>
#include <poll.h>

using namespace std;

int main(void) {

  // wpisz wszystkie kategorie
  vector<string> categories = {"IMIE", "NAZWISKO", "MIASTO", "ROSLINA"};

  int serverId = create_server_socket(3333);
  if (serverId == -1) {
    return 1;
  }

  Gamestate gameState;
  gameState.gameStarted = false;
  gameState.random_letter = '\0';

  gameState.players = vector<Player>();

  vector<vector<string>> messages;
  mutex message_mutex;

  vector<int> clientIds;

  pollfd pfds[1];
  pfds[0].fd = serverId;
  pfds[0].events = POLLIN;

  bool isCountingDown = false;
  chrono::steady_clock::time_point countdownStart;
  int lastPrintedSecond = -1;
  constexpr int COUNTDOWN_SECONDS = 5;

  while (true) {

    int poll_result = poll(pfds, 1, 100);
    if(poll_result < 0) {    //if poll fails
      perror("Poll failed");
      break;
    }
    else if(poll_result == 1) { //if there is a new connection
      
    int clientId = accept_client(serverId);
    if (clientId == -1) {
      continue;
    }
    clientIds.push_back(clientId);
    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              ref(gameState), categories);
    t1.detach();
    }  
    lock_guard<mutex> lock(gameState.m);
    if (!gameState.gameStarted) {
      const size_t totalPlayers = gameState.players.size();
      const size_t readyPlayers = static_cast<size_t>(
          count_if(gameState.players.begin(), gameState.players.end(),
                   [](const Player &p) { return p.ready; }));
      const bool hasMajority =
          (totalPlayers >= 2 && readyPlayers * 2 > totalPlayers);

      if (hasMajority) {
        if (!isCountingDown) {
          isCountingDown = true;
          countdownStart = chrono::steady_clock::now();
          lastPrintedSecond = COUNTDOWN_SECONDS + 1;
          cout << "Rozpoczynanie odliczania do startu gry..." << endl;
        }

        const auto elapsed = chrono::duration_cast<chrono::seconds>(
                                 chrono::steady_clock::now() - countdownStart)
                                 .count();
        const int remaining = COUNTDOWN_SECONDS - static_cast<int>(elapsed);

        if (remaining > 0) {
          if (remaining < lastPrintedSecond) {
            cout << remaining << "..." << endl;
            lastPrintedSecond = remaining;
          }
        } else {
          char next_letter;
          do {
            next_letter = generateRandomChar();
          } while (next_letter == gameState.random_letter);
          gameState.random_letter = next_letter;
          cout << "Wylosowana litera: " << gameState.random_letter << endl;
          cout << "Gra rozpoczeta!" << endl;
          gameState.gameStarted = true;
          isCountingDown = false;
          lastPrintedSecond = -1;
        }
      } else if (isCountingDown) {
        isCountingDown = false;
        lastPrintedSecond = -1;
        cout << "Odliczanie przerwane - brak wiekszosci gotowych graczy!"
             << endl;
      }
      continue;
    }
    if(gameState.gameStarted && gameState.players.empty()){
      display_answers(messages);
      messages.clear();
      gameState.gameStarted = false;
    }

    
  }
  close_socket(serverId);

  return 0;
}
