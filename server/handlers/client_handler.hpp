#pragma once

#include <vector>
#include <string>
#include <mutex>

using namespace std;

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, char random_letter,
                  vector<string> categories);
