#pragma once

#include "../client_handler.hpp"

bool waitForGameStart(int clientId, std::string &accumulatedText,
                      Gamestate &gameState);
