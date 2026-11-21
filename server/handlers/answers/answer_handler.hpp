#pragma once

#include <string>
#include <vector>
#include <mutex>

std::vector<std::string> collectAnswers(
    int clientId, std::string &accumulatedText, char randomLetter,
    const std::vector<std::string> &categories);

void saveAnswers(std::vector<std::string> answers, const std::string &nickname,
                 std::vector<std::vector<std::string>> &messages,
                 std::mutex &messageMutex);
