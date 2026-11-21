#pragma once

#include "../client_handler.hpp"

bool registerPlayer(int clientId, std::string &nickname, Gamestate &gameState);
void removePlayer(int clientId, Gamestate &gameState);
