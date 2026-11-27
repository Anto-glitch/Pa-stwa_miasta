#include "player_registry.hpp"

#include <algorithm>

#include "../../systems/socket_message.hpp"

bool registerPlayer(int clientId, std::string &nickname, Gamestate &gameState) {
  if (nickname.empty()) {
    nickname = "Gracz_" + std::to_string(clientId);
  }

  std::lock_guard<std::mutex> lock(gameState.m);
  const auto existingPlayer = std::find_if(
      gameState.players.begin(), gameState.players.end(),
      [&nickname](const Player &player) {
        return player.nickname == nickname;
      });

  if (existingPlayer != gameState.players.end()) {
    write_to_socket(clientId, "BAD_NICKNAME\n");
    return false;
  }

  gameState.players.push_back({clientId, nickname, false, false, false, false});
  return true;
}

void removePlayer(int clientId, Gamestate &gameState) {
  std::lock_guard<std::mutex> lock(gameState.m);
  gameState.players.erase(
      std::remove_if(gameState.players.begin(), gameState.players.end(),
                     [clientId](const Player &player) {
                       return player.socket_id == clientId;
                     }),
      gameState.players.end());
}
