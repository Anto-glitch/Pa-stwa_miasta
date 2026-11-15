#include "read_line.hpp"

#include "../../systems/socket_message.hpp"

ReadStatus readLineFromSocket(int clientId, std::string &accumulatedText,
                              std::string &outLine, int flags) {
  char buffer[1024];
  while (true) {
    const std::size_t position = accumulatedText.find('\n');
    if (position != std::string::npos) {
      outLine = accumulatedText.substr(0, position);
      if (!outLine.empty() && outLine.back() == '\r') {
        outLine.pop_back();
      }
      accumulatedText.erase(0, position + 1);
      return ReadStatus::SUCCESS;
    }

    const ssize_t bytesRead =
        read_from_socket(clientId, buffer, sizeof(buffer) - 1, flags);
    if (bytesRead <= 0) {
      return flags == 1 ? ReadStatus::PARTIAL : ReadStatus::ERROR;
    }

    accumulatedText.append(buffer, bytesRead);
    if (flags == 1 && accumulatedText.find('\n') == std::string::npos) {
      return ReadStatus::PARTIAL;
    }
  }
}
