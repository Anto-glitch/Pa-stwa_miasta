#include "utills.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstddef>

using namespace std;

void display_answers(vector<vector<string>> &messages) {
  for (size_t i = 0; i < messages.size(); i++) {
    cout << "Gracz " << i + 1 << ": ";
    for (size_t j = 0; j < messages[i].size(); j++) {
      cout << "[" << messages[i][j] << "] ";
    }
    cout << endl;
  }
}