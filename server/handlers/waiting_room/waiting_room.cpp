#include "waiting_room.hpp"

#include <algorithm>
#include <cstdio>
#include <poll.h>

#include "../io/read_line.hpp"

namespace { //sprawdzić czemu xdd

bool markPlayerReady(int clientId, Gamestate &gameState) {
  std::lock_guard<std::mutex> lock(gameState.m);
  const auto player = std::find_if(
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

bool isGameStarted(Gamestate &gameState) {
  std::lock_guard<std::mutex> lock(gameState.m);
  return gameState.gameStarted;
}

}

bool waitForGameStart(int clientId, std::string &accumulatedText,
                      Gamestate &gameState) {
  std::string ClientCommand;

  // Na słuchuj na rozpoczęcie gry i jednoczesnie na  komendy od klienta

  pollfd pollDescriptor{clientId, POLLIN, 0};
  while (!isGameStarted(gameState)) {
    pollDescriptor.revents = 0;
    if (poll(&pollDescriptor, 1, 100) < 0) {
      perror("Poll failed");
      return false;
    }

    if (pollDescriptor.revents & (POLLHUP | POLLERR | POLLNVAL)) {
      return false;
    }
    if (pollDescriptor.revents & POLLIN) {
      const ReadStatus status =
          readLineFromSocket(clientId, accumulatedText, ClientCommand, 1);
      if (status == ReadStatus::ERROR ||
          (status == ReadStatus::SUCCESS && ClientCommand == "EXIT")) {
        return false;
      }
      else if (status == ReadStatus::SUCCESS && ClientCommand == "READY") {
        if (!markPlayerReady(clientId, gameState)) {
          return false;
        }
      }
    }
  }

  return true;
}
