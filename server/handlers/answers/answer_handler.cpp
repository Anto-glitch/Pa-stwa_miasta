#include "answer_handler.hpp"

#include <iostream>

#include "../io/read_line.hpp"
#include "../../systems/socket_message.hpp"
#include "../../utils/utils.hpp"

std::vector<std::string> collectAnswers(
    int clientId, std::string &accumulatedText, char randomLetter,
    const std::vector<std::string> &categories, Gamestate &gameState) {
  std::vector<std::string> answers;
  std::size_t categoryIndex = 0;

  const auto votingStarted = [&gameState] {
    std::lock_guard<std::mutex> lock(gameState.m);
    return gameState.roundState == RoundState::VOTING;
  };

  if (categories.empty()) {
    return answers;
  }

  while (categoryIndex < categories.size()) {
    if (votingStarted()) {
      break;
    }

    const std::string prompt =
        "Podaj " + categories[categoryIndex] +
        " (kazde zatwierdz enterem):\n";
    write_to_socket(clientId, prompt);

    std::string line;
    const ReadStatus status =
        readLineFromSocket(clientId, accumulatedText, line, 1);
    if (status == ReadStatus::ERROR ||
        (status == ReadStatus::SUCCESS && line == "EXIT")) {
      break;
    }
    if (status != ReadStatus::SUCCESS) {
      continue;
    }
    if (votingStarted()) {
      break;
    }

    line = capitalize(line);
    if (line.empty() || line[0] != randomLetter) {
      continue;
    }

    answers.push_back(line);
    ++categoryIndex;
  }
  write_to_socket(clientId,"RESULTS");
  return answers;
}

void saveAnswers(std::vector<std::string> answers, const std::string &nickname,
                 std::vector<std::vector<std::string>> &messages,
                 std::mutex &messageMutex) {
  if (answers.empty()) {
    return;
  }

  std::lock_guard<std::mutex> lock(messageMutex);
  answers.insert(answers.begin(), nickname);
  messages.push_back(answers);

  std::cout << "Ilosc zatwierdzonych pakietow odpowiedzi: " << messages.size()
            << std::endl;
  std::cout << "Zestawienie odpowiedzi wszystkich graczy: " << std::endl;
  display_answers(messages);
}
