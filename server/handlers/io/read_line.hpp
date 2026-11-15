#pragma once

#include <string>

enum class ReadStatus {
  SUCCESS,
  ERROR,
  PARTIAL
};

ReadStatus readLineFromSocket(int clientId, std::string &accumulatedText,
                              std::string &outLine, int flags = 0);
